/*
    This file is part of etherwall.
    etherwall is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    etherwall is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with etherwall. If not, see <http://www.gnu.org/licenses/>.
*/
/** @file etheripc.h
 * @author Ales Katona <almindor@gmail.com>
 * @date 2015
 *
 * Ethereum IPC client header
 */

#ifndef ETHERIPC_H
#define ETHERIPC_H

#include <QObject>
#include <QList>
#include <QLocalSocket>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QTimer>
#include <QThread>
#include "types.h"

namespace Etherwall {

    enum RequestBurden {
        Full,
        NonVisual,
        None
    };

    class RequestIPC {
    public:
        RequestIPC(RequestBurden burden, RequestTypes type, const QString method, const QJsonArray params = QJsonArray(), int index = -1);
        RequestIPC(RequestTypes type, const QString method, const QJsonArray params = QJsonArray(), int index = -1);
        RequestIPC(RequestBurden burden);
        RequestIPC();

        RequestTypes getType() const;
        const QString& getMethod() const;
        const QJsonArray& getParams() const;
        int getIndex() const;
        int getCallID() const;
        RequestBurden burden() const;
        static int sCallID;
    private:
        int fCallID;
        RequestTypes fType;
        QString fMethod;
        QJsonArray fParams;
        int fIndex;
        RequestBurden fBurden;
    };

    typedef QList<RequestIPC> RequestList;

    class EtherIPC: public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString error READ getError NOTIFY error)
        Q_PROPERTY(int code READ getCode NOTIFY error)
        Q_PROPERTY(bool busy READ getBusy NOTIFY busyChanged)
        Q_PROPERTY(int connectionState READ getConnectionState NOTIFY connectionStateChanged)
        Q_PROPERTY(quint64 peerCount READ peerCount NOTIFY peerCountChanged)
    public:
        EtherIPC();
        void setWorker(QThread* worker);
        bool getBusy() const;
        const QString& getError() const;
        int getCode() const;
    public slots:
        void connectToServer(const QString& path);
        void connectedToServer();
        void disconnectedFromServer();
        void getAccounts();
        bool refreshAccount(const QString& hash, int index);
        void newAccount(const QString& password, int index);
        void deleteAccount(const QString& hash, const QString& password, int index);
        void getBlockNumber();
        void getPeerCount();
        void sendTransaction(const QString& from, const QString& to, double value, double gas = -1);
        void unlockAccount(const QString& hash, const QString& password, int duration, int index);
        void getGasPrice();
        void getTransactionByHash(const QString& hash);
        void getBlockByHash(const QString& hash);
        void getBlockByNumber(quint64 blockNum);
        void onSocketReadyRead();
        void onSocketError(QLocalSocket::LocalSocketError err);
        Q_INVOKABLE void setInterval(int interval);
        bool closeApp();
    signals:
        void connectToServerDone();
        void newAccountDone(const QString& result, int index);
        void deleteAccountDone(bool result, int index);
        void getBlockNumberDone(quint64 num);
        void sendTransactionDone(const QString& hash);
        void unlockAccountDone(bool result, int index);
        void getGasPriceDone(const QString& price);
        void newTransaction(const TransactionInfo& info);
        void newBlock(const QJsonObject& block);

        void peerCountChanged(quint64 num);
        void accountChanged(const AccountInfo& info);
        void busyChanged(bool busy);
        void connectionStateChanged();
        void error();
    private:
        QLocalSocket fSocket;
        int fPendingTransactionsFilterID;
        int fBlockFilterID;
        bool fClosingApp;
        quint64 fPeerCount;
        QByteArray fReadBuffer;
        QString fError;
        int fCode;
        QString fPath;
        AccountList fAccountList;
        TransactionList fTransactionList;
        RequestList fRequestQueue;
        RequestIPC fActiveRequest;
        QTimer fTimer;

        void handleNewAccount();
        void handleDeleteAccount();
        void handleAccountDetails();
        void handleAccountBalance();
        void handleAccountTransactionCount();
        void handleGetBlockNumber();
        void handleGetPeerCount();
        void handleSendTransaction();
        void handleUnlockAccount();
        void handleGetGasPrice();
        void handleFilter(int& filterID);
        void handleGetFilterChanges();
        void handleUninstallFilter();
        void handleGetTransactionByHash();
        void handleGetBlock();

        void onTimer();
        void getFilterChanges(RequestTypes subRequest, int filterID);
        int getConnectionState() const;
        quint64 peerCount() const;
        void bail();
        void done();
        void newPendingTransactionFilter();
        void newBlockFilter();
        void uninstallFilter(int filterID);

        QJsonObject methodToJSON(const RequestIPC& request);
        bool queueRequest(const RequestIPC& request);
        bool writeRequest();
        bool readReply(QJsonValue& result);
        bool readVin(BigInt::Vin& result);
        bool readNumber(quint64& result);
        const QString toDecStr(const QJsonValue& jv) const;
    };

}

#endif // ETHERIPC_H

