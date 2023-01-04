#ifndef FORMMODSCA_H
#define FORMMODSCA_H

#include <QWidget>
#include <QTimer>
#include "enums.h"
#include "modbusclient.h"
#include "displaydefinition.h"
#include "modbuswriteparams.h"

class MainWindow;

namespace Ui {
class FormModSca;
}

///
/// \brief The FormModSca class
///
class FormModSca : public QWidget
{
    Q_OBJECT

public:
    explicit FormModSca(int num, ModbusClient& client, MainWindow* parent);
    ~FormModSca();

    DisplayDefinition displayDefinition() const;
    void setDisplayDefinition(const DisplayDefinition& dd);

    DisplayMode displayMode() const;
    void setDisplayMode(DisplayMode mode);

    DataDisplayMode dataDisplayMode() const;
    void setDataDisplayMode(DataDisplayMode mode);

    bool displayHexAddreses() const;
    void setDisplayHexAddreses(bool on);

    void resetCtrs();

private slots:
    void on_timeout();
    void on_modbusReply(QModbusReply* reply);
    void on_modbusRequest(int requestId, const QModbusRequest& request);
    void on_lineEditAddress_valueChanged(const QVariant&);
    void on_lineEditLength_valueChanged(const QVariant&);
    void on_lineEditDeviceId_valueChanged(const QVariant&);
    void on_comboBoxModbusPointType_pointTypeChanged(QModbusDataUnit::RegisterType);
    void on_outputWidget_itemDoubleClicked(quint32 addr, const QVariant& value);

private:
    Ui::FormModSca *ui;
    QTimer _timer;
    int _formId;
    ModbusClient& _modbusClient;
};

#endif // FORMMODSCA_H
