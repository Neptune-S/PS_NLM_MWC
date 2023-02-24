/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Bertrand Martel
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
package fr.bmartel.protocol.websocket.server;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketException;

import fr.bmartel.protocol.http.HttpFrame;
import fr.bmartel.protocol.http.constants.HttpConstants;
import fr.bmartel.protocol.http.constants.HttpHeader;
import fr.bmartel.protocol.http.states.HttpStates;
import fr.bmartel.protocol.websocket.WebSocketChannel;
import fr.bmartel.protocol.websocket.WebSocketHandshake;
import fr.bmartel.protocol.websocket.listeners.IClientEventListener;

/**
 * <b>Server socket connection management</b>
 * 
 * @author Bertrand Martel
 */
public class ServerSocketChannel implements Runnable, IWebsocketClient {

	/** websocket decoder / encoder object */
	private WebSocketChannel websocketChannel = null;

	/** socket to be used by server */
	private Socket socket;

	/** inputstream to be used for reading */
	private InputStream inputStream;

	/** outputstream to be used for writing */
	private OutputStream outputStream;

	/** http request parser */
	private HttpFrame httpFrameParser;

	private IClientEventListener clientListener = null;
	//private final static String boardClientIP = "192.168.3.187";
	private final static String boardClientIP = "192.168.2.203";
	private final static int boardClientPort = 8080;
	private volatile boolean running = true;
	private static Socket boardClientSocket = null;
	String content;
	String line = "";
	private boolean loopbreak = true;

	/**
	 * Initialize socket connection when the connection is available ( socket
	 * parameter wil block until it is opened)
	 * 
	 * @param socket  the socket opened
	 * @param context the current OSGI context
	 */
	public ServerSocketChannel(Socket socket, IClientEventListener clientListener) {
		try {
			this.clientListener = clientListener;

			/* give the socket opened to the main class */
			this.socket = socket;
			/* extract the associated input stream */
			this.inputStream = socket.getInputStream();
			/* extract the associated output stream */
			this.outputStream = socket.getOutputStream();

			/*
			 * initialize parsing method for request string and different body of http
			 * request
			 */
			this.httpFrameParser = new HttpFrame();
			/*
			 * intialize response manager for writing data to outputstream method (headers
			 * generation ...)
			 */

		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * Write byte array to socket outputstream
	 * 
	 * @param bytes
	 * @throws IOException
	 */
	private void writeToSocket(byte[] bytes) throws IOException {
		synchronized (this.socket.getOutputStream()) {
			this.socket.getOutputStream().write(bytes);
			this.socket.getOutputStream().flush();
		}
	}

	/**
	 * Define if websocket connection has been enables by client and server
	 */
	private boolean websocket = false;

	/**
	 * Main socket thread : parse all datas passing through socket inputstream
	 */
	@Override
	public void run() {
		try {
			do {
				/* clear richRequest object (specially headers) */
				this.httpFrameParser = new HttpFrame();

				/*
				 * define loop if websocket has been enables by client and server
				 */
				if (websocket == false) {

					HttpStates httpStatus = this.httpFrameParser.parseHttp(inputStream);

					if (httpStatus == HttpStates.HTTP_FRAME_OK) {

						/* check if Connection: Upgrade is present in header map */
						if (this.httpFrameParser.getHeaders().containsKey(HttpHeader.CONNECTION.toLowerCase())
								&& this.httpFrameParser.getHeaders().containsKey(HttpHeader.UPGRADE.toLowerCase())) {
							if (this.httpFrameParser.getHeaders().get(HttpHeader.CONNECTION.toLowerCase()).toLowerCase()
									.indexOf("upgrade") != -1
									&& this.httpFrameParser.getHeaders().get(HttpHeader.UPGRADE.toLowerCase())
											.toLowerCase().indexOf("websocket") != -1) {

								upgradeWebsocketProtocol(this.outputStream, this.httpFrameParser, this);

								websocketChannel = new WebSocketChannel();
								notifyConnectionSuccess();
								this.websocket = true;
							}
						} else if (httpStatus == HttpStates.MALFORMED_HTTP_FRAME) {

							writeToSocket(HttpConstants.BAD_REQUEST_ERROR.getBytes("UTF-8"));

						} else {

							websocket = false;
							closeSocket();
							return;
						}
					}
				} else {

					/* read something on websocket stream */
					byte[] data = this.websocketChannel.decapsulateMessage(this.inputStream);

					String messageRead = "";

					if (data != null) {
						messageRead = new String(data);
					} else {
						websocket = false;
						closeSocket();
						return;
					}

					if (clientListener != null && messageRead != null) {
						clientListener.onMessageReceivedFromClient(this, messageRead);
					}

					if (messageRead == null) {
						websocket = false;
						closeSocket();
						return;
					}

				}
			} while (websocket == true);
			closeSocket();
		} catch (SocketException e) {
		} catch (Exception e) {
			e.printStackTrace();
			// TODO : redirect ?
		}
	}

	/**
	 * Switch to Websocket protocol from server socket
	 * 
	 * @param out             socket outputStream
	 * @param httpFrameParser http parser
	 * @param serverThread    server thread
	 * @throws UnsupportedEncodingException
	 * @throws IOException
	 */
	private void upgradeWebsocketProtocol(OutputStream out, HttpFrame httpFrameParser, ServerSocketChannel serverThread)
			throws UnsupportedEncodingException, IOException {

		/* write websocket handshake to client */
		synchronized (out) {
			out.write(WebSocketHandshake.writeWebSocketHandShake(httpFrameParser).getBytes());
			out.flush();
		}
	}

	/**
	 * Notify websocket client connection success
	 */
	private void notifyConnectionSuccess() {
		if (clientListener != null) {
			clientListener.onClientConnection(this);
		}
	}

	/**
	 * Close socket inputstream
	 * 
	 * @throws IOException
	 */
	private void closeInputStream() throws IOException {
		this.inputStream.close();
	}

	/**
	 * Close socket inputstream
	 */
	private void closeOutputStream() throws IOException {
		this.outputStream.close();
	}

	private void closeSocket() {
		try {
			closeInputStream();
			closeOutputStream();
			this.socket.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	@Override
	public int close() {
		System.out.println("closing ....");

		websocket = false;
		closeSocket();

		if (clientListener != null) {
			clientListener.onClientClose(this);
		}

		return 0;
	}

	@Override
	public int sendMessage(String message) {
		if (outputStream != null) {
			try {
				websocketChannel.encapsulateMessage(message, outputStream);
				return 0;
			} catch (IOException | InterruptedException e) {
				// e.printStackTrace();
			}
		}
		return -1;
	}
	
	

	@Override
	public String sendAndReceiveMessageFromCClient(String message, IWebsocketClient client) {
		try {

			if (boardClientSocket == null) {

				boardClientSocket = new Socket(boardClientIP, boardClientPort);
				System.out.println("Connection with board established" + boardClientSocket.isConnected()
						+ boardClientSocket.isBound() + boardClientSocket.isClosed());
			}

			boardClientSocket.setReuseAddress(true);
			boardClientSocket.setKeepAlive(true);

			// sending data to board
			DataOutputStream out = new DataOutputStream(boardClientSocket.getOutputStream());
		
			// String result = message.substring(1, message.length() - 1);
			int i = Integer.parseInt(message);
			// String inputJson = "\n{\"msg_type\":1,\"gscn\":7911}";
			String inputJson = "";
			

			switch (i) {
			case 0:
				inputJson = "\n{\"msg_type\":9,\"gscn\":7911}";
				out.writeBytes(inputJson);
				loopbreak = true;
				break;
			case 1:
				inputJson = "\n{\"msg_type\":1,\"gscn\":7911}";
				out.writeBytes(inputJson);
				loopbreak = true;
				break;
			case 2:
				inputJson = "\n{\"msg_type\":2,\"gscn\":7911}";
				out.writeBytes(inputJson);
				loopbreak = true;
				break;
			case 3:
				inputJson = "\n{\"msg_type\":3,\"gscn\":7911}";
				out.writeBytes(inputJson);
				loopbreak = false;
				break;
			default:
				System.out.println("Msg type or gscn  not found");
				loopbreak = false;
			}

			// out.writeBytes(inputJson);
			System.out.println("Message sent to board: " + inputJson);

			// reading data from board
			StringBuilder sb = new StringBuilder();

			try (BufferedReader in = new BufferedReader(new InputStreamReader(boardClientSocket.getInputStream()))) {
				//line = in.readLine();
				while(true) {
					// while (in.ready()) {
					line = in.readLine();
					// if (!line.isBlank() && !line.isEmpty())
					if (line != null || !"".equals(line))
						sb.delete(0, sb.length());
					sb.append(line).append(System.lineSeparator());
					// }

					content = sb.toString();
					// if (!sb.isEmpty())
					if (sb != null) {
						System.out.println(content);

						// sending message to websocket
						if (!message.equals("3")) {
							client.sendMessage(content);
						}
						sb.delete(0, sb.length());
					}
				}
			} catch (Exception e) {
				System.out.println("No data to read from board");
				e.printStackTrace();
			}

		} catch (IOException e) {
			System.out.println("Either connection to board is not established or message is not send to board");
			e.printStackTrace();
		} 
			 
		return content;
	}
}