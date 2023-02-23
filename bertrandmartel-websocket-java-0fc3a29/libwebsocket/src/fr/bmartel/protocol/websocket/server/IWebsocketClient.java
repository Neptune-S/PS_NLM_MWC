package fr.bmartel.protocol.websocket.server;

import java.net.Socket;

public interface IWebsocketClient {
	
	/**
	 *  close websoclet client object
	 *  
	 * @return
	 * 		0 if success -1 if error
	 */
	public int close();

	/**
	 * Send a message to websocket client
	 * 
	 * @param string
	 * 		Message to be sent to client
	 * @return
	 *		0 if success -1 if error 
	 */
	public int sendMessage(String message);
	
	public String sendAndReceiveMessageFromCClient (String message, IWebsocketClient client);
	
}
