#include <QKeyEvent>
#include "modbusfunction.h"
#include "enums.h"
#include "formatutils.h"
#include "qhexvalidator.h"
#include "quintvalidator.h"
#include "functioncodecombobox.h"

///
/// \brief FunctionCodeComboBox::FunctionCodeComboBox
/// \param parent
///
FunctionCodeComboBox::FunctionCodeComboBox(QWidget *parent)
    : QComboBox(parent)
    ,_currentFunc(QModbusPdu::Invalid)
{
    setInputMode(InputMode::DecMode);

    connect(this, &QComboBox::currentTextChanged, this, &FunctionCodeComboBox::on_currentTextChanged);
    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &FunctionCodeComboBox::on_currentIndexChanged);
}

///
/// \brief FunctionCodeComboBox::currentFunctionCode
/// \return
///
QModbusPdu::FunctionCode FunctionCodeComboBox::currentFunctionCode() const
{
    return _currentFunc;
}

///
/// \brief FunctionCodeComboBox::setCurrentFunctionCode
/// \param pointType
///
void FunctionCodeComboBox::setCurrentFunctionCode(QModbusPdu::FunctionCode funcCode)
{
    _currentFunc = funcCode;
    const auto mode = (_inputMode == InputMode::HexMode) ? DataDisplayMode::Hex : DataDisplayMode::Decimal;
    const auto idx = findData(funcCode);
    if(idx != -1) setCurrentIndex(idx);
    else setCurrentText(formatFuncCode(mode, funcCode));
}

///
/// \brief FunctionCodeComboBox::inputMode
/// \return
///
FunctionCodeComboBox::InputMode FunctionCodeComboBox::inputMode() const
{
    return _inputMode;
}

///
/// \brief FunctionCodeComboBox::setInputMode
/// \param on
///
void FunctionCodeComboBox::setInputMode(FunctionCodeComboBox::InputMode mode)
{
    _inputMode = mode;

    switch(mode)
    {
        case DecMode:
           // setValidator(new QUIntValidator(0, 255, this));
        break;

        case HexMode:
           // setValidator(new QHexValidator(0, 0xFF, this));
        break;
    }

    update();
}

///
/// \brief FunctionCodeComboBox::addItem
/// \param funcCode
///
void FunctionCodeComboBox::addItem(QModbusPdu::FunctionCode funcCode)
{
    const auto mode = (_inputMode == InputMode::HexMode) ? DataDisplayMode::Hex : DataDisplayMode::Decimal;
    const auto code = formatFuncCode(mode, funcCode);
    QComboBox::addItem(QString("%1: %2").arg(code, ModbusFunction(funcCode)), funcCode);
}

///
/// \brief FunctionCodeComboBox::addAllItems
///
void FunctionCodeComboBox::addAllItems()
{
    static const auto items = {
        QModbusPdu::ReadCoils, QModbusPdu::ReadDiscreteInputs, QModbusPdu::ReadHoldingRegisters, QModbusPdu::ReadInputRegisters,
        QModbusPdu::WriteSingleCoil, QModbusPdu::WriteSingleRegister, QModbusPdu::ReadExceptionStatus, QModbusPdu::Diagnostics,
        QModbusPdu::GetCommEventCounter, QModbusPdu::GetCommEventLog, QModbusPdu::WriteMultipleCoils, QModbusPdu::WriteMultipleRegisters,
        QModbusPdu::ReportServerId, QModbusPdu::ReadFileRecord, QModbusPdu::WriteFileRecord, QModbusPdu::MaskWriteRegister,
        QModbusPdu::ReadWriteMultipleRegisters, QModbusPdu::ReadFifoQueue, QModbusPdu::EncapsulatedInterfaceTransport
    };

    for(auto&& item : items)
        addItem(item);
}

///
/// \brief FunctionCodeComboBox::update
///
void FunctionCodeComboBox::update()
{
    const auto mode = (_inputMode == InputMode::HexMode) ? DataDisplayMode::Hex : DataDisplayMode::Decimal;

    if(currentData(Qt::DisplayRole) != currentText())
    {
        const auto code = formatFuncCode(mode, currentFunctionCode());
        setCurrentText(code);
    }

    for (int i = 0; i < count(); i++)
    {
        const auto funcCode = itemData(i).value<QModbusPdu::FunctionCode>();
        const auto code = formatFuncCode(mode, funcCode);
        setItemText(i, QString("%1: %2").arg(code, ModbusFunction(funcCode)));
    }
}

///
/// \brief FunctionCodeComboBox::on_currentIndexChanged
/// \param index
///
void FunctionCodeComboBox::on_currentIndexChanged(int index)
{
    _currentFunc = itemData(index).value<QModbusPdu::FunctionCode>();
    emit functionCodeChanged(_currentFunc);
}

///
/// \brief FunctionCodeComboBox::on_currentTextChanged
///
void FunctionCodeComboBox::on_currentTextChanged(const QString& text)
{
     if(currentData(Qt::DisplayRole) == text)
        _currentFunc = currentData().value<QModbusPdu::FunctionCode>();
    else
    {
        bool ok;
        quint8 func;

        switch(_inputMode)
        {
            case InputMode::DecMode:
                func = text.toUInt(&ok);
                break;

            case InputMode::HexMode:
                func = text.toUInt(&ok, 16);
                break;
        }

        _currentFunc = ok ? (QModbusPdu::FunctionCode)func : QModbusPdu::Invalid;
    }
}

///
/// \brief FunctionCodeComboBox::focusOutEvent
/// \param e
///
void FunctionCodeComboBox::focusOutEvent(QFocusEvent* e)
{
    update();
    QComboBox::focusOutEvent(e);
}
