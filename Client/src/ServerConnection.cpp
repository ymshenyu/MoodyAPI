#include "ServerConnection.hpp"

#include "MoodyAPI.grpc.pb.h"
#include "MoodyAPI.pb.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QThread>
#include <QtDebug>
#include <grpcpp/grpcpp.h>

using namespace std::chrono_literals;

ServerConnection::ServerConnection() : QThread(), m_noTls(false)
{
}

void ServerConnection::SetServerInfo(const QString &host, const QString &secret, bool noTls)
{
    m_serverAddr = host.trimmed();
    m_secret = secret.trimmed();
    m_noTls = noTls;
    m_serverInfoChanged.storeRelaxed(true);
    if (m_pollingContext)
        m_pollingContext->TryCancel();
}

ServerConnection::~ServerConnection()
{
}

void ServerConnection::run()
{
    while (true)
    {
        emit onConnectionStatusChanged(false);
        m_channel = grpc::CreateChannel(m_serverAddr.toStdString(), m_noTls ? grpc::InsecureChannelCredentials() : grpc::SslCredentials({}));

        m_isServerConnected = m_channel->WaitForConnected(std::chrono::system_clock::now() + 1s); //, true && !m_isServerConnected && ++retry < 5)

        if (!m_isServerConnected)
        {
            QThread::sleep(1);
            continue;
        }

        m_serverInfoChanged.storeRelaxed(false);
        auto serverStub = MoodyAPI::MoodyAPIService::NewStub(m_channel);

        while (!m_serverInfoChanged.loadRelaxed())
        {
            m_pollingContext.reset(new grpc::ClientContext);

            MoodyAPI::SubscribeCameraStateChangeRequest request;
            request.mutable_auth()->set_clientuuid(m_secret.toStdString());

            auto reader = serverStub->SubscribeCameraStateChange(m_pollingContext.get(), request);

            MoodyAPI::CameraState resp;
            while (reader->Read(&resp) && true)
            {
                emit onConnectionStatusChanged(true);
                emit onCameraStateChanged(resp.state());
            }

            qDebug() << QDateTime::currentDateTime() << "Cannot read more responses, retry.";
            m_pollingContext->TryCancel();
            QThread::sleep(1);
        }

        emit onConnectionStatusChanged(false);
        QThread::sleep(1);
    }
}

void ServerConnection::SetCameraState(bool newState)
{
    if (!m_isServerConnected)
        return;

    grpc::ClientContext m_pollingContext;
    auto serverStub = MoodyAPI::MoodyAPIService::NewStub(m_channel);

    MoodyAPI::UpdateCameraStateRequest request;
    request.mutable_auth()->set_clientuuid(m_secret.toStdString());
    request.mutable_state()->set_state(newState);

    ::google::protobuf::Empty empty;
    serverStub->UpdateCameraState(&m_pollingContext, request, &empty);
}
