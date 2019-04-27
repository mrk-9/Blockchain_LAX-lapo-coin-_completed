#include <QTimer>
#include <QtWidgets>
#include <QString>
#include <QByteArray>
#include <QClipboard>
#include <univalue.h>
#include <utiltime.h>

#include "masternodewizard.h"
#include "masternodeconfig.h"
#include "base58.h"
#include "key.h"
#include "rpcserver.h"
#include "util.h"
#include "guiutil.h"

MasternodeWizard::MasternodeWizard(QWidget *parent)
    : QWizard(parent)
{
    setPage(Page_Intro, new IntroPage);
    setPage(Page_Encrypted, new EncryptedPage);
    setPage(Page_Collateral, new CollateralPage);
    setPage(Page_IpAddress, new IpAddressPage);
    setPage(Page_Conclusion, new ConclusionPage);

//    setPixmap(QWizard::BannerPixmap, QPixmap(":/images/banner.png"));
//    setPixmap(QWizard::BackgroundPixmap, QPixmap(":/images/background.png"));

    setWindowTitle(tr("Masternode Wizard"));
}

void MasternodeWizard::accept()
{
    QString alias = field("alias").toString();
    QString ipAddress = field("ipAddress").toString();
    ipAddress.append(QString::fromStdString(":31714"));
    QString privKey = field("privKey").toString();
    QString txHash = field("txHash").toString();
    QString idx = field("idx").toString();
    
    if (privKey.isNull() || privKey.isEmpty()) {
        // just accept and quit
        QDialog::accept();
        return;
    }
    
    QString conf = "";
    conf += alias;
    conf += " ";
    conf += ipAddress;
    conf += " ";
    conf += privKey;
    conf += " ";
    conf += txHash;
    conf += " ";
    conf += idx;
    conf += "\n";
    
    printf("%s", conf.toStdString().c_str());
    
    // string GetWalletPath(const CWallet& wallet)
    // {
    //     // Copy wallet.dat
    //     filesystem::path pathSrc = GetDataDir() / wallet.strWalletFile;
    //     return pathSrc.string();
    // }
    

    // 1. Get path
    boost::filesystem::path pathMasternodeConfigFile = GetMasternodeConfigFile();
    
    // 1.9 Check if the file ends with a newline
    QFile mnConfFile(QString::fromStdString(pathMasternodeConfigFile.string()));
    if (!mnConfFile.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(0, QObject::tr("Masternode Wizard"),
                             QObject::tr("Cannot read file %1:\n%2")
                             .arg(mnConfFile.fileName())
                             .arg(mnConfFile.errorString()));
        return;    
    }
    
    QByteArray byteArray = mnConfFile.readAll();
    if (byteArray.size() > 1 && byteArray[byteArray.size()-1] != '\n' && byteArray.size() > 2 && byteArray[byteArray.size()-2] != '\n')
    {
//        printf("adding an extra newline to masternode.conf\n");
        conf = "\n" + conf;
    }
    mnConfFile.close();
    
    // 2. append
    if (!mnConfFile.open(QFile::WriteOnly | QFile::Text | QIODevice::Append)) {
        QMessageBox::warning(0, QObject::tr("Masternode Wizard"),
                             QObject::tr("Cannot write file %1:\n%2")
                             .arg(mnConfFile.fileName())
                             .arg(mnConfFile.errorString()));
        return;
    }    
    mnConfFile.write(conf.toStdString().c_str());
    mnConfFile.close();
        
    QDialog::accept();

    // 3. reboot - how?
    QStringList args;
    emit handleRestart(args, 0);
}

void MasternodeWizard::setBitcoinGUI(BitcoinGUI* gui)
{
    if (gui) {
        connect(this, SIGNAL(handleRestart(QStringList, int)), gui, SLOT(handleRestart(QStringList, int)));
    }
}

IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Introduction"));
//    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark1.png"));
//    wizard()->button(QWizard::BackButton)->setEnabled(false);

    introPageLabel = new QLabel();
    syncLabel = new QLabel();
    
    introPageLabel->setText(tr("This wizard will create a new masternode entry "
                          "for your wallet. Five parameters will be generated: "
                          "a transaction hash (TxHash), an output index (Idx), "
                          "a name, an ip address, and a pre-shared private key."));
    introPageLabel->setWordWrap(true);
    syncLabel->setWordWrap(true);
    
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(introPageLabel);
    layout->addWidget(syncLabel);
    setLayout(layout);
}

void IntroPage::initializePage() 
{
    if (isSynced()) 
    {
        syncLabel->setText(tr("Your wallet is synchronized. Please proceed."));
        customNextId = MasternodeWizard::Page_Encrypted;
    }
    else 
    {
        QString finishText = wizard()->buttonText(QWizard::FinishButton);
        finishText.remove('&');
        syncLabel->setText(tr("Your wallet is NOT synchronized. Please "
              "press %1 to cancel the wizard and return when the wallet "
              "is ready.")
          .arg(finishText));
    
    
        customNextId = -1;
    }
}

int IntroPage::nextId() const
{
    return customNextId;
}

bool IntroPage::isSynced()
{
    UniValue params(UniValue::VARR);
    UniValue result;

    // before sending, we must check if we are synced
    // https://bitcoin.stackexchange.com/questions/37815/how-to-know-if-bitcoind-synced
    result = getblockcount(params, false);
    if (result.isNum()) {
        int blockCount = result.get_int();
//        printf("block count: %d\n", blockCount);
        
        params.push_back(blockCount);
        result = getblockhash(params, false);

        if (result.isStr()) {
            string blockHash = result.get_str();
//            printf("block hash: %s\n", blockHash.c_str());
            
            params.clear();
            params.setArray();
            params.push_back(blockHash);
            result = getblock(params, false);
            if (result.isObject()) {
                result = find_value(result, "time");
                if (result.isNum()) {
                    int64_t nTime = result.get_int64();
//                    printf("block time: %lld\n", nTime);
//                    printf("block diff: %llds\n", GetTime() - nTime);
                    
                    if (nTime + 5 * 60 > GetTime())    // 5 minutes
                        return true;
                }
            }
            
        }        
    }    
    
    return false;
}

EncryptedPage::EncryptedPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Encryption Check"));
    
    label = new QLabel;
    label->setWordWrap(true);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(label, 0,0,1,2);
    
    if (!isWalletEncrypted()) {
        label->setText(tr("Your wallet is not encrypted."));
        customNextId = MasternodeWizard::Page_Collateral;
    } else {
        label->setText(tr("Your wallet is encrypted, you must enter your passphrase to continue."));
        customNextId = MasternodeWizard::Page_Collateral;
//        customNextId = -1;
        passPhraseLabel = new QLabel(tr("&Passphrase:"));

        passPhraseLineEdit = new QLineEdit;
        passPhraseLineEdit->setEchoMode(QLineEdit::Password);
        passPhraseLabel->setBuddy(passPhraseLineEdit);

        registerField("passPhrase*", passPhraseLineEdit);

        layout->addWidget(passPhraseLabel, 1, 0);
        layout->addWidget(passPhraseLineEdit, 1, 1);
    }
        
    setLayout(layout);
}
    
void EncryptedPage::initializePage() 
{

}

bool EncryptedPage::isWalletEncrypted()
{
    // find a way to check for locked wallet, how?
    return isWalletCrypted();
}


int EncryptedPage::nextId() const
{
    return customNextId;
}

CollateralPage::CollateralPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Masternode Collateral"));
//    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark1.png"));
//    setCommitPage(true);   // remove the Go Back button on the next page

//    wizard()->button(QWizard::NextButton)->setEnabled(false);

    label = new QLabel;
    // collateralLabel = new QLabel;
    // outputsLabel = new QLabel;
    // outputsTodoLabel = new QLabel;
    refreshLabel = new QLabel;

    label->setWordWrap(true);
    refreshLabel->setWordWrap(true);
    //outputsTodoLabel->setWordWrap(true);

    txLabel = new QLabel(tr("&TxHash:"));
    txLineEdit = new QLineEdit;
    txLabel->setBuddy(txLineEdit);

    idxLabel = new QLabel(tr("&Idx:"));
    idxLineEdit = new QLineEdit;
    idxLabel->setBuddy(idxLineEdit);
    idxLineEdit->setValidator(new QRegExpValidator( QRegExp("[0-9]"), this ));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(label, 0,0,1,2);
    // layout->addWidget(collateralLabel, 1,0,1,2);
    // layout->addWidget(outputsLabel, 2,0,1,2);
    // layout->addWidget(outputsTodoLabel, 3,0,1,2);
    layout->addWidget(refreshLabel, 4,0,1,2);
    layout->addWidget(txLabel, 5, 0);
    layout->addWidget(txLineEdit, 5, 1);
    layout->addWidget(idxLabel, 6, 0);
    layout->addWidget(idxLineEdit, 6, 1);

    setLayout(layout);
}

void CollateralPage::initializePage() 
{        
    if (hasCollateral()) {
        label->setText(tr("Collateral detected. Selecting first available transaction."));
        customNextId = MasternodeWizard::Page_IpAddress;
//        wizard()->button(QWizard::NextButton)->setEnabled(true);

    }
    else if (hasSufficientBalance())
    {
        label->setText(tr("Unable to detect existing collateral. Funds will "
                          "be transferred to a new account. Awaiting confirmation (~60 seconds). "
                          "Please wait for Tx and Idx to appear before pressing continue."));
        QTimer::singleShot(1000, this, SLOT(launchCollateralCheck()));
        customNextId = MasternodeWizard::Page_IpAddress;
    }
    else 
    {
        label->setText(tr("Unable to detect existing collateral. Your balance is "
                                     "insufficient. Please rerun Wizard when your available "
                                     "balance exceeds the collateral amount."));
        customNextId = -1;
    }
}

void CollateralPage::launchCollateralCheck()
{
    // transfer, run and re-initialize
    if (setupCollateral()) {
        // timer, and then intialize page            
        QTimer::singleShot(1000, this, SLOT(checkCollateral()));
    }    
}

void CollateralPage::checkCollateral()
{
    static int refreshCount = 10;
    
    refreshCount--;
    
    if (refreshCount == 0) {
            refreshCount = 10;
            if (hasCollateral())
            {
                label->setText(tr("Collateral detected. Selecting first valid transaction."));
                customNextId = MasternodeWizard::Page_IpAddress;
                refreshLabel->setText(tr(""));
        //        wizard()->button(QWizard::NextButton)->setEnabled(true);
                return;
            }
    }
    else {
        refreshLabel->setText(tr("Refreshing in %1").arg(refreshCount));
    }
    QTimer::singleShot(1000, this, SLOT(checkCollateral()));
}

bool CollateralPage::hasCollateral() 
{
    UniValue params(UniValue::VARR);
    
    UniValue result = getmasternodeoutputs(params, false);
    if (result.isArray()) {         // it should be
        UniValue outputs = result.get_array();
        int nOutputs = outputs.size();
//        printf("# of masternode outputs: %d\n", nOutputs);
        
        for (int i = 0; i < nOutputs; i++) {                
            string txHash = find_value(outputs[i],"txhash").get_str();
            int idx = find_value(outputs[i],"outputidx").get_int();
                            
            UniValue listParams(UniValue::VARR);
            listParams.push_back(txHash); // filter on txHash
            
            result = listmasternodeconf(listParams, false);
            if (result.isArray()) {
                UniValue matches = result.get_array();
                int nMatches = matches.size();
                
//                printf("%d matches %d\n",i, nMatches);
                if (nMatches == 0)  //unused collateral
                {
                    registerField("txHash*", txLineEdit);
                    registerField("idx*", idxLineEdit);
                    
                    txLineEdit->setText(QString::fromStdString(txHash));
                    idxLineEdit->setText(QString::number(idx));
                    return true;
                }
            }
        }
    }

    return false;    
}

bool CollateralPage::hasSufficientBalance() 
{
    UniValue params(UniValue::VARR);
    
    UniValue result = getbalance(params, false);
    if (result.isNum()) {
        double balance = result.get_real();
        if (balance > 1000000) {
            return true;
        }
    }
    
    return false;
}

bool CollateralPage::setupCollateral() 
{
    UniValue params(UniValue::VARR);
    UniValue result;
    
    std::ostringstream ss;
    ss << "mn_" << GetTime();
    string accountName = ss.str();

//    printf("accountName: %s\n", accountName.c_str());
    // we need to unlock for 10 seconds
    if (isWalletCrypted()) {
        try {
            QString passPhrase = field("passPhrase").toString();
            //printf("passPhrase: %s\n", passPhrase.toStdString().c_str());
            
            UniValue passParams(UniValue::VARR);

            passParams.push_back(passPhrase.toStdString());
            passParams.push_back(5);
            result = walletpassphrase(passParams, false);
        }
        catch (const UniValue& objError) {
            UniValue message = find_value(objError, "message");
            label->setText(QString::fromStdString(message.get_str()) + " Press Go Back to re-enter passphrase.");
            customNextId = -1;
            return false;
        }
    }
        
    params.push_back(accountName);
    result = getaccountaddress(params, false);
    if (result.isStr()) {
        string newAccount = result.get_str();
        UniValue sendParams(UniValue::VARR);
        sendParams.push_back(newAccount);
        sendParams.push_back(1000000);      // again get from chain params if possible (or write into RPC?)

        try {
            result = sendtoaddress(sendParams, false);
            if (result.isStr()) {
                string receipt = result.get_str();

                if (isWalletCrypted()) {
                    // lets lock it up
                    UniValue lockParams(UniValue::VARR);
                    walletlock(lockParams, false);
                }
                
                return true;
                // outputsTodoLabel->setText(tr("Unable to detect existing collateral. Funds have "
                //                                  "been transferred to a new account. Awaiting confirmation..."));
            }
        } catch (const UniValue& objError) {
//            printf("caught error\n");
            UniValue message = find_value(objError, "message");;
//            printf("got message\n");
            if (message.get_str() == "Transaction too large") {
//                printf("reporting message\n");
                refreshLabel->setText(tr("Transaction too large. Please consolidate your oldest transactions (e.g., send 500k to new account), then return to the wizard."));
            }
            else {                
                refreshLabel->setText(QString::fromStdString(message.get_str()));                
            }
            customNextId = -1;
        } catch (const std::exception& e) {
//            printf("caught error\n");
            std::string strReply = JSONRPCReply(NullUniValue, JSONRPCError(RPC_PARSE_ERROR, e.what()), 0);
            refreshLabel->setText(QString::fromStdString(strReply));
            customNextId = -1;
        }
    }
    return false;
}

int CollateralPage::nextId() const
{
//    printf("evaluating nextId - maybe just once\n");
    return customNextId;
}


IpAddressPage::IpAddressPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Masternode Address"));
    setSubTitle(tr("Please enter a new masternode alias and ip address below."));
    
    aliasLabel = new QLabel(tr("&Name:"));
    aliasLineEdit = new QLineEdit;
    aliasLabel->setBuddy(aliasLineEdit);

    // not existing name (or else it overwrites)

    ipAddressLabel = new QLabel(tr("&Ip address:"));
    ipAddressIpCtrl = new IPCtrl;
    ipAddressLabel->setBuddy(ipAddressIpCtrl);

    // not existing ipaddress (or i think thats bad)

    registerField("alias*", aliasLineEdit);
    registerField("ipAddress*", ipAddressIpCtrl->outputLineEdit);
    
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(aliasLabel, 0, 0);
    layout->addWidget(aliasLineEdit, 0, 1);
    layout->addWidget(ipAddressLabel, 1, 0);
    layout->addWidget(ipAddressIpCtrl, 1, 1);
//    layout->addWidget(ipAddressIpCtrl->outputLineEdit, 2, 1);
    setLayout(layout);
}

void IpAddressPage::initializePage() 
{    
    QStringList aliases;
    QStringList addresses;
    
    UniValue listParams(UniValue::VARR);
    UniValue result = listmasternodeconf(listParams, false);
    if (result.isArray()) // it will be
    {
        for (size_t i = 0; i < result.size(); i++) {
            UniValue obj = result[i];
            aliases << QString::fromStdString(find_value(obj, "alias").get_str());
            addresses << QString::fromStdString(find_value(obj, "address").get_str()).replace(":31714","").replace(".","\\."); // may need to escape the period for regex
        }
    }
    
    QString strAliases = aliases.join("|");
    QString strAddresses = addresses.join("|");
    
    // printf("excluding %s\n", strAliases.toStdString().c_str());
    // printf("excluding %s\n", strAddresses.toStdString().c_str());
    
//    string reAliases = "[A-Za-z0-9_.]{1,20}";
    string reAliases = "^(?!(" + strAliases.toStdString() + ")$)[A-Za-z0-9_]{1,20}$";
    string reAddresses = "^(?!(" + strAddresses.toStdString() + ")$)[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$";
    
    // printf("aliases: %s\n", reAliases.c_str());
    // printf("ip: %s\n", reAddresses.c_str());
    
    aliasLineEdit->setValidator(new LenientRegExpValidator( QRegularExpression(reAliases.c_str()), this ));
    ipAddressIpCtrl->outputLineEdit->setValidator(new QRegExpValidator( QRegExp(reAddresses.c_str()), this ));


    std::ostringstream ss;
    ss << "mn" << GetTime();
    string name = ss.str();
    
    // how about add the date time?
    aliasLineEdit->setText(QString::fromStdString(name));
}

ConclusionPage::ConclusionPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Conclusion"));
//    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark2.png"));

    label = new QLabel;
    copyButton = new QToolButton;

    privKeyLabel = new QLabel(tr("&Shared key:"));
    privKeyLineEdit = new QLineEdit;
    privKeyLabel->setBuddy(privKeyLineEdit);
    registerField("privKey*", privKeyLineEdit);

    label->setWordWrap(true);
    
    copyButton->setToolTip(tr("Copy the shared key to the system clipboard"));
    copyButton->setIcon(QIcon(":/icons/editcopy"));    
    connect(copyButton, SIGNAL (released()),this, SLOT (on_copyButton_clicked()));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(label, 0, 0, 1, 3);
    layout->addWidget(privKeyLabel, 1, 0);
    layout->addWidget(privKeyLineEdit, 1, 1);
    layout->addWidget(copyButton, 1, 2);
    setLayout(layout);
}

void ConclusionPage::initializePage()
{
    QString privKey = getPrivKey();
    privKeyLineEdit->setText(privKey);

    QString finishText = wizard()->buttonText(QWizard::FinishButton);
    finishText.remove('&');
    label->setText(tr("Please copy the pre-shared key below and then click %1 "
                      "to update the local masternode.conf file and restart the wallet. "
                      "If you have a different pre-shared key from the server script, you "
                      "may replace the one below.")
                   .arg(finishText));
}

void ConclusionPage::on_copyButton_clicked()
{
//    printf("on_copyButton_clicked\n");
    GUIUtil::setClipboard(privKeyLineEdit->text());
}

QString ConclusionPage::getPrivKey() 
{
    UniValue params(UniValue::VARR);

    UniValue result = createmasternodekey(params, false);    
    if (result.isStr()) {
        string strPrivKey = result.get_str(); 
        return QString::fromStdString(strPrivKey);
    }
    
    return QString();  // should never get here
}
