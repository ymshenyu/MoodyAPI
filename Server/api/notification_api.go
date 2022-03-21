package api

import (
	context "context"
	"errors"
	"log"
	"time"

	"api.mooody.me/common"
	"api.mooody.me/models"
	emptypb "google.golang.org/protobuf/types/known/emptypb"
)

func (s *MoodyAPIServer) BroadcastNotification(event *models.Notification) {
	s.notificationBroadcaster.Broadcast(event)
}

func (s *MoodyAPIServer) SendNotification(_ context.Context, request *models.SendNotificationRequest) (*emptypb.Empty, error) {
	if request == nil || request.Auth == nil || request.Auth.ClientId != common.APISecret {
		log.Printf("WARNING: Invalid secret from client: %s", request.Auth.ClientId)
		return &emptypb.Empty{}, errors.New("error: Invalid Secret")
	}

	println("Send Notification: ", request.Notification.Title, request.Notification.Message)
	s.BroadcastNotification(request.Notification)
	return &emptypb.Empty{}, nil
}

func (s *MoodyAPIServer) SubscribeNotifications(request *models.SubscribeNotificationsRequest, server models.MoodyAPIService_SubscribeNotificationsServer) error {
	log.Printf("New notification client connected")
	if request == nil || request.Auth == nil || request.Auth.ClientId != common.APISecret {
		log.Printf("WARNING: Invalid secret from client: %s", request.Auth.ClientId)
		return errors.New("error: Invalid Secret")
	}

	subscribeId := time.Now().UnixNano()
	eventChannel, err := s.notificationBroadcaster.Subscribe(subscribeId)
	if err != nil {
		return err
	}

done:
	for {
		select {
		case signal := <-eventChannel:
			{
				resp := signal.(*models.Notification)
				server.Send(resp)
			}
		case <-server.Context().Done():
			{
				log.Printf("Client disconnected")
				break done
			}
		}
	}

	s.notificationBroadcaster.Unsubscribe(subscribeId)

	return nil
}
