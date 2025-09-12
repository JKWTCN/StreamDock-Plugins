#ifndef ACTION_H
#define ACTION_H

#include "SDKDefines.h"
#include <QJsonObject>
#include <QString>

class ConnectionManager;

class Action
{
public:
    Action(ConnectionManager *connection, const QString &action, const QString &context);
    virtual ~Action();

    // Get action information
    QString GetAction() const;
    QString GetContext() const;

protected:
    // Get connection manager
    ConnectionManager *GetConnectionManager() const;

public:
    // Events Received
    virtual void DidReceiveSettings(const QJsonObject &payload);
    virtual void KeyDown(const QJsonObject &payload);
    virtual void KeyUp(const QJsonObject &payload);
    virtual void DialUp(const QJsonObject &payload);
    virtual void DialDown(const QJsonObject &payload);
    virtual void SendToPlugin(const QJsonObject &payload);
    virtual void WillAppear(const QJsonObject &payload);
    virtual void WillDisappear(const QJsonObject &payload);
    virtual void RotateClockwise(const QJsonObject &payload, const unsigned int ticks, const bool pressed);
    virtual void RotateCounterClockwise(const QJsonObject &payload, const unsigned int ticks, const bool pressed);

protected:
    // Events Sent
    void SetState(int state);
    void SetTitle(const QString &title, mSDKTarget = mSDKTarget_HardwareAndSoftware, int state = -1);
    void SetImage(const QString &inBase64ImageString, mSDKTarget = mSDKTarget_HardwareAndSoftware, int state = -1);
    void ShowAlert();
    void ShowOK();
    void SetSettings(const QJsonObject &inPayload);
    void SendToPropertyInspector(const QJsonObject &inPayload);

private:
    QString mAction;
    QString mContext;
    ConnectionManager *mConnectionManager = nullptr;
};

#endif // ACTION_H
