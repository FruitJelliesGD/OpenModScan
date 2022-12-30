#include <QtWidgets>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include "dialogdisplaydefinition.h"
#include "dialogconnectiondetails.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

///
/// \brief MainWindow::MainWindow
/// \param parent
///
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,_modbusClient(nullptr)
{
    ui->setupUi(this);
    setUnifiedTitleAndToolBarOnMac(true);

    auto dispatcher = QAbstractEventDispatcher::instance();
    connect(dispatcher, &QAbstractEventDispatcher::awake, this, &MainWindow::on_awake);

    connect(ui->mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::updateMenus);

    ui->actionNew->trigger();
}

///
/// \brief MainWindow::~MainWindow
///
MainWindow::~MainWindow()
{
    delete ui;
}

///
/// \brief MainWindow::closeEvent
/// \param event
///
void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->mdiArea->closeAllSubWindows();
    if (ui->mdiArea->currentSubWindow())
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void MainWindow::on_awake()
{
    auto frm = currentMdiChild();
    const auto state = _modbusClient ? _modbusClient->state() : QModbusDevice::UnconnectedState;

    ui->actionSave->setEnabled(frm != nullptr);
    ui->actionSaveAs->setEnabled(frm != nullptr);
    ui->actionConnect->setEnabled(state == QModbusDevice::UnconnectedState);
    ui->actionDisconnect->setEnabled(state == QModbusDevice::ConnectedState);
    ui->actionQuickConnect->setEnabled(state == QModbusDevice::UnconnectedState);
    ui->actionDataDefinition->setEnabled(frm != nullptr);
    ui->actionShowData->setEnabled(frm != nullptr);
    ui->actionShowTraffic->setEnabled(frm != nullptr);
    ui->actionBinary->setEnabled(frm != nullptr);
    ui->actionUnsignedDecimal->setEnabled(frm != nullptr);
    ui->actionInteger->setEnabled(frm != nullptr);
    ui->actionHex->setEnabled(frm != nullptr);
    ui->actionFloatingPt->setEnabled(frm != nullptr);
    ui->actionSwappedFP->setEnabled(frm != nullptr);
    ui->actionDblFloat->setEnabled(frm != nullptr);
    ui->actionSwappedDbl->setEnabled(frm != nullptr);
    ui->actionResetCtrs->setEnabled(frm != nullptr);
    ui->actionPrint->setEnabled(frm != nullptr);

    if(frm != nullptr)
    {
        const auto ddm = frm->dataDisplayMode();
        ui->actionBinary->setChecked(ddm == DataDisplayMode::Binary);
        ui->actionUnsignedDecimal->setChecked(ddm == DataDisplayMode::Decimal);
        ui->actionInteger->setChecked(ddm == DataDisplayMode::Integer);
        ui->actionHex->setChecked(ddm == DataDisplayMode::Hex);
        ui->actionFloatingPt->setChecked(ddm == DataDisplayMode::FloatingPt);
        ui->actionSwappedFP->setChecked(ddm == DataDisplayMode::SwappedFP);
        ui->actionDblFloat->setChecked(ddm == DataDisplayMode::DblFloat);
        ui->actionSwappedDbl->setChecked(ddm == DataDisplayMode::SwappedDbl);

        const auto dm = frm->displayMode();
        ui->actionShowData->setChecked(dm == DisplayMode::Data);
        ui->actionShowTraffic->setChecked(dm == DisplayMode::Traffic);
    }
}

///
/// \brief MainWindow::on_actionNew_triggered
///
void MainWindow::on_actionNew_triggered()
{
    auto frm = createMdiChild();
    //frm->setDataDisplayMode(_settings.DataDisplayMode);
    frm->show();
}

///
/// \brief MainWindow::on_actionConnect_triggered
///
void MainWindow::on_actionConnect_triggered()
{
    DialogConnectionDetails dlg(_settings.ConnectionParams, this);
    if(dlg.exec() == QDialog::Accepted)
    {
        setupModbusClient(_settings.ConnectionParams);
        ui->actionQuickConnect->trigger();
    }
}

///
/// \brief MainWindow::on_actionDisconnect_triggered
///
void MainWindow::on_actionDisconnect_triggered()
{
    if(_modbusClient != nullptr)
    {
        _modbusClient->disconnectDevice();
    }
}

///
/// \brief MainWindow::on_actionQuickConnect_triggered
///
void MainWindow::on_actionQuickConnect_triggered()
{
    if(_modbusClient == nullptr)
        setupModbusClient(_settings.ConnectionParams);

    _modbusClient->connectDevice();
}

///
/// \brief MainWindow::on_actionDataDefinition_triggered
///
void MainWindow::on_actionDataDefinition_triggered()
{
     auto frm = currentMdiChild();
     if(frm)
     {
        DialogDisplayDefinition dlg(frm);
        dlg.exec();
     }
}

///
/// \brief MainWindow::on_actionShowData_triggered
///
void MainWindow::on_actionShowData_triggered()
{
    auto frm = currentMdiChild();
    if(frm) frm->setDisplayMode(DisplayMode::Data);
}

///
/// \brief MainWindow::on_actionShowTraffic_triggered
///
void MainWindow::on_actionShowTraffic_triggered()
{
    auto frm = currentMdiChild();
    if(frm) frm->setDisplayMode(DisplayMode::Traffic);
}

///
/// \brief MainWindow::on_actionBinary_triggered
///
void MainWindow::on_actionBinary_triggered()
{
    updateDataDisplayMode(DataDisplayMode::Binary);
}

///
/// \brief MainWindow::on_actionUnsignedDecimal_triggered
///
void MainWindow::on_actionUnsignedDecimal_triggered()
{
    updateDataDisplayMode(DataDisplayMode::Decimal);
}

///
/// \brief MainWindow::on_actionInteger_triggered
///
void MainWindow::on_actionInteger_triggered()
{
    updateDataDisplayMode(DataDisplayMode::Integer);
}

///
/// \brief MainWindow::on_actionHex_triggered
///
void MainWindow::on_actionHex_triggered()
{
    updateDataDisplayMode(DataDisplayMode::Hex);
}

///
/// \brief MainWindow::on_actionFloatingPt_triggered
///
void MainWindow::on_actionFloatingPt_triggered()
{
    updateDataDisplayMode(DataDisplayMode::FloatingPt);
}

///
/// \brief MainWindow::on_actionSwappedFP_triggered
///
void MainWindow::on_actionSwappedFP_triggered()
{
    updateDataDisplayMode(DataDisplayMode::SwappedFP);
}

///
/// \brief MainWindow::on_actionDblFloat_triggered
///
void MainWindow::on_actionDblFloat_triggered()
{
    updateDataDisplayMode(DataDisplayMode::DblFloat);
}

///
/// \brief MainWindow::on_actionSwappedDbl_triggered
///
void MainWindow::on_actionSwappedDbl_triggered()
{
    updateDataDisplayMode(DataDisplayMode::SwappedDbl);
}

///
/// \brief MainWindow::on_actionResetCtrs_triggered
///
void MainWindow::on_actionResetCtrs_triggered()
{
    auto frm = currentMdiChild();
    if(frm) frm->resetCtrs();
}

///
/// \brief MainWindow::updateMenus
///
void MainWindow::updateMenus()
{
}

///
/// \brief MainWindow::setupModbusClient
/// \param cd
///
void MainWindow::setupModbusClient(const ConnectionDetails& cd)
{
    if(_modbusClient != nullptr)
    {
        delete _modbusClient;
        _modbusClient = nullptr;
    }

    switch(cd.Type)
    {
        case ConnectionType::Tcp:
        {
            _modbusClient = new QModbusTcpClient(this);
            _modbusClient->setTimeout(cd.ModbusParams.SlaveResponseTimeOut);
            _modbusClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, cd.TcpParams.IPAddress);
            _modbusClient->setConnectionParameter(QModbusDevice::NetworkPortParameter, cd.TcpParams.ServicePort);

            emit modbusClientChanged(_modbusClient);
        }
        break;

        case ConnectionType::Serial:
            _modbusClient = new QModbusRtuSerialMaster(this);
            _modbusClient->setTimeout(cd.ModbusParams.SlaveResponseTimeOut);
            _modbusClient->setProperty("DelayBetweenPolls", cd.ModbusParams.DelayBetweenPolls);
            _modbusClient->setConnectionParameter(QModbusDevice::SerialPortNameParameter, cd.SerialParams.PortName);
            _modbusClient->setConnectionParameter(QModbusDevice::SerialParityParameter, cd.SerialParams.Parity);
            _modbusClient->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, cd.SerialParams.BaudRate);
            _modbusClient->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, cd.SerialParams.WordLength);
            _modbusClient->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, cd.SerialParams.StopBits);

            emit modbusClientChanged(_modbusClient);
        break;
    }

    connect(_modbusClient, &QModbusDevice::errorOccurred, this,
            [&](QModbusDevice::Error error)
            {
                if(error == QModbusDevice::ConnectionError)
                {
                    const auto errorString = QString("Connection error. %1").arg(_modbusClient->errorString());
                    QMessageBox::warning(this, windowTitle(), errorString);
                }
            });
}

///
/// \brief MainWindow::updateDisplayMode
/// \param mode
///
void MainWindow::updateDataDisplayMode(DataDisplayMode mode)
{
    //_settings.DataDisplayMode = mode;
    auto frm = currentMdiChild();
    if(frm) frm->setDataDisplayMode(mode);
}

///
/// \brief MainWindow::createMdiChild
/// \return
///
FormModSca* MainWindow::createMdiChild()
{
    const auto num = ui->mdiArea->subWindowList().count() + 1;
    auto frm = new FormModSca(num, _modbusClient, this);
    ui->mdiArea->addSubWindow(frm);
    return frm;
}

///
/// \brief MainWindow::currentMdiChild
/// \return
///
FormModSca* MainWindow::currentMdiChild()
{
    auto child = ui->mdiArea->currentSubWindow();
    return child ? (FormModSca*)child->widget() : nullptr;
}
