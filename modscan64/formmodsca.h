#ifndef FORMMODSCA_H
#define FORMMODSCA_H

#include <QWidget>
#include <QTimer>
#include <QModbusClient>
#include "enums.h"
#include "displaydefinition.h"

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
    explicit FormModSca(int num, QModbusClient* client, MainWindow* parent = nullptr);
    ~FormModSca();

    DisplayDefinition displayDefinition() const;
    void setDisplayDefinition(const DisplayDefinition& dd);

    DisplayMode displayMode() const;
    void setDisplayMode(DisplayMode mode);

    DataDisplayMode dataDisplayMode() const;
    void setDataDisplayMode(DataDisplayMode mode);

    void resetCtrs();

private slots:
    void readyReadData();
    void on_timeout();
    void on_lineEditAddress_valueChanged(int);
    void on_lineEditLength_valueChanged(int);
    void on_lineEditDeviceId_valueChanged(int);
    void on_comboBoxModbusPointType_currentTextChanged(const QString&);

private:
    void sendReadRequest();

private:
    Ui::FormModSca *ui;
    QModbusClient* _modbusClient;
    QTimer _timer;
};

#endif // FORMMODSCA_H
