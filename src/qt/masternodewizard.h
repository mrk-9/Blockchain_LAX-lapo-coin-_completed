#ifndef MASTERNODEWIZARD_H
#define MASTERNODEWIZARD_H

#include <QWizard>
#include <QWidget>
#include "rpcserver.h"
#include "ipctrl.h"
#include "bitcoingui.h"

class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;

namespace Ui
{
class MasternodeWizard;
}

class MasternodeWizard : public QWizard
{
    Q_OBJECT

public:
    MasternodeWizard(QWidget *parent = 0);
    enum { Page_Intro, Page_Encrypted, Page_Collateral, Page_IpAddress, 
        //Page_ClassInfo, Page_CodeStyle, Page_OutputFiles,
        Page_Conclusion };
    void setBitcoinGUI(BitcoinGUI* gui);

    void accept() override;
    
signals:
    void handleRestart(QStringList args, int action);
};

class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = 0);

protected:
    void initializePage() override;
    int nextId() const override;
    int customNextId;
    
private:
    QLabel *introPageLabel;
    QLabel *syncLabel;    
    bool isSynced();
};

class EncryptedPage : public QWizardPage
{
    Q_OBJECT
    
public:
    EncryptedPage(QWidget *parent = 0);
    
protected:
    void initializePage() override;
    int nextId() const override;
    int customNextId;
    
private:
    QLabel *label;
    QLabel *passPhraseLabel;
    QLineEdit *passPhraseLineEdit;
    bool isWalletEncrypted();
};

class CollateralPage : public QWizardPage
{
    Q_OBJECT

public:
    CollateralPage(QWidget *parent = 0);

public slots:
    void checkCollateral();
    void launchCollateralCheck();

protected:
    void initializePage() override;
    int nextId() const override;
    int customNextId;
    
private:
    QLabel *label;
    // QLabel *collateralLabel;
    // QLabel *outputsLabel;
    // QLabel *outputsTodoLabel;
    QLabel *refreshLabel;
    QLabel *txLabel;
    QLineEdit *txLineEdit;
    QLabel *idxLabel;
    QLineEdit *idxLineEdit;
    bool hasCollateral();
    bool hasSufficientBalance();
    bool setupCollateral();
};

// See https://forum.qt.io/topic/85305/validation-behaviour-too-confusing-for-end-user/6 for issue
class LenientRegExpValidator : public QRegularExpressionValidator
{
    Q_OBJECT
    Q_DISABLE_COPY(LenientRegExpValidator)

public:
    LenientRegExpValidator(QObject* parent = Q_NULLPTR) : QRegularExpressionValidator(parent){}
    LenientRegExpValidator(const QRegularExpression &re, QObject *parent = Q_NULLPTR) : QRegularExpressionValidator(re,parent){}
    QValidator::State validate(QString &input, int &pos) const Q_DECL_OVERRIDE 
    {
            const QValidator::State baseValidator = QRegularExpressionValidator::validate(input,pos);
            if(baseValidator ==QValidator::Invalid)
            return QValidator::Intermediate;
            return baseValidator;
    }
};

class IpAddressPage : public QWizardPage
{
    Q_OBJECT

public:
    IpAddressPage(QWidget *parent = 0);

protected:
    void initializePage() override;

private:
    QLabel *label;
    QLabel *aliasLabel;
    QLineEdit *aliasLineEdit;
    QLabel *ipAddressLabel;
    IPCtrl *ipAddressIpCtrl;
};

class ConclusionPage : public QWizardPage
{
    Q_OBJECT

public:
    ConclusionPage(QWidget *parent = 0);

private slots:
    void on_copyButton_clicked();

protected:
    void initializePage() override;

private:
    QLabel *label;
    QLabel *privKeyLabel;
    QLineEdit *privKeyLineEdit;
    QToolButton *copyButton;
    QString getPrivKey();
};

#endif // MASTERNODEWIZARD_H
