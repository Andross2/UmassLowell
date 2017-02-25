import java.io.*;
import java.nio.*;
import java.nio.channels.*;
import java.util.Random;
import java.net.*;

class UDPClient {
	// ACK value
	public static byte ACK = 1;
	// ACK packet size
	public static int ackPacketSize = 7;
	// UDP Packet data size is limited to 1KB
	public static int udpPktSize = 1024;
	// checksum size is 2 bytes(16 bit) in UDP
	public static int checkSumSize = 2;
	// sequence + checksum size
	public static int additionalSize = 6;
	public static InetAddress IPAddress;
	public static int port = 9876;
	public static DatagramSocket clientSocket;
	public static int sequenceNo = 0;
	public static int sequenceNoRec = 0;
	// Selected the percentage of packets to be corrupted
	public static int percentBitError = 0;
	// Selected the percentage of packets to be corrupted
	public static int percentPacketLoss = 0;
	// random number generator
	public static Random rand = new Random();
	// random number generator
	public static Random rand2 = new Random();
	// Window Size of Go-Back-N
	public static int windowSize = 50;
	// Window base
	public static int windowBase = 0;
	// Receiver non-blocking Timer value
	public static int receiveTimer = 15;
	//starting timer value
	public static long startTimerValue = 0;
	//packet timeout value
	public static long packetTimeout = 25;
	
	//To start the timer
	public static void start_timer()
	{
		startTimerValue = System.currentTimeMillis();
	}
	//To stop the timer
	public static void stop_timer()
	{
		startTimerValue = 0;
	}
	//To check for time out
	public static boolean timer_timeout()
	{
		if(Math.abs(System.currentTimeMillis()-startTimerValue)>=packetTimeout)
			return true;
		return false;
	}
	// To find the received packet is not corrupted.
	public static boolean notcorrupt(byte[] data) {
		try {
			short checkSum = 0;
			// convert data array to byte buffer
			ByteBuffer dataBuf = ByteBuffer.wrap(data);
			while (dataBuf.remaining() > 1) {
				checkSum = (short) ((checkSum) ^ (dataBuf.getShort()));
			}
			if (dataBuf.remaining() == 1) {
				checkSum = (short) ((checkSum) ^ (0x0F & dataBuf.get()));
			}
			if (checkSum == 0)
				return true;
			return false;
		} catch (Exception e) {
			System.out.println("Error:" + e);
		}
		return false;
	}

	// check for correct acknowledgement received
	public static boolean isACK(int seqNo, byte[] data) {
		if (data == null)
			return false;
		try {
			int seqTmp = 0;
			short chkSumtmp = 0;
			byte ackTmp = 0;
			// convert data array to byte buffer
			ByteBuffer dataBuf = ByteBuffer.wrap(data);
			seqTmp = dataBuf.getInt();
			sequenceNoRec = seqTmp;
			//System.out.println(seqTmp);
			chkSumtmp = dataBuf.getShort();
			ackTmp = dataBuf.get();
			if ((notcorrupt(data)) && (ackTmp == ACK))
				return true;
			return false;
		} catch (Exception e) {
			System.out.println("Error:" + e);
		}
		return false;
	}

	// To calculate checksum
	public static short CheckSum(int seqNo, byte[] data) {
		try {
			// Convert sequence number to byte buffer
			ByteBuffer seqNoBuf = ByteBuffer.allocate(4);
			seqNoBuf.putInt(seqNo);
			seqNoBuf.rewind();
			short checkSum = seqNoBuf.getShort();
			checkSum = (short) ((checkSum) ^ (seqNoBuf.getShort()));
			// convert data array to byte buffer
			ByteBuffer dataBuf = ByteBuffer.wrap(data);
			while (dataBuf.remaining() > 1) {
				checkSum = (short) ((checkSum) ^ (dataBuf.getShort()));
			}
			if (dataBuf.remaining() == 1) {
				checkSum = (short) ((checkSum) ^ (0x0F & dataBuf.get()));
			}
			return checkSum;
		} catch (Exception e) {
			System.out.println("Error:" + e);
		}
		return -1;
	}

	// To frame a packet for given data
	public static byte[] make_pkt(int seqNo, byte[] data, short checkSum) {
		// Initialize byte array for framing packet
		ByteBuffer packetBuf = ByteBuffer.allocate(data.length + additionalSize);
		int randTmp = rand.nextInt(100) + 1;
		// System.out.println("rand:"+randTmp);
		if (randTmp <= percentBitError)
			packetBuf.putInt(-seqNo);
		else
			packetBuf.putInt(seqNo);
		// System.out.println(seqNo);
		packetBuf.putShort(checkSum);
		packetBuf.put(data);
		packetBuf.rewind();
		byte[] packet = packetBuf.array();
		return packet;
	}

	// To send the through datagram socket
	public static void udt_send(byte[] sendData) {
		try {
			// Get Ip address of the host
			IPAddress = InetAddress.getByName("localhost");
			// Send the data from byte buffer through the socket
			DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, port);
			clientSocket.send(sendPacket);
		} catch (IOException e) {
			System.out.println("Error:" + e);
		}
	}

	// To break the file into 1KB and send
	public static void rdt_send(ByteBuffer data) {
		long startCount = System.currentTimeMillis();
		try {
			// Open Client Socket
			clientSocket = new DatagramSocket();
		} catch (IOException e) {
			System.out.println("Error:" + e);
		}
		try {
			byte[] packetData = new byte[udpPktSize];
			byte[] packet = null;
			// Read bytes from byte buffer to the byte array
			while (true) {
				// Send the Window
				if (data.remaining() != 0) {
					while ((sequenceNo >= windowBase) && (sequenceNo < (windowBase + windowSize))) {
						if (data.remaining() >= udpPktSize) {
							data.get(packetData);
							packet = make_pkt(sequenceNo, packetData, CheckSum(sequenceNo, packetData));
							int randTmp2 = rand2.nextInt(100) + 1;
							if (!(randTmp2 <= percentPacketLoss))
							{
								udt_send(packet);
							}
						} else {
							byte[] packetDataLast = new byte[data.remaining()];
							data.get(packetDataLast);
							packet = make_pkt(sequenceNo, packetDataLast, CheckSum(sequenceNo, packetDataLast));
							udt_send(packet);
						}
						if(sequenceNo==windowBase)
						{
							start_timer();
						}
						sequenceNo++;
					}
				} else {
					if (windowBase >= sequenceNo)
						break;
				}
				// Wait for ACK
				byte[] ackRec = rdt_rcv(ackPacketSize);
				if (isACK(windowBase, ackRec)) {
					if ((sequenceNoRec >= windowBase) && (sequenceNoRec < (windowBase + windowSize))) {
						windowBase = sequenceNoRec + 1;
						if(windowBase==sequenceNo)
							stop_timer();
						else
							start_timer();
					}
				} else {
					//Resends the window for only timeout
					if (timer_timeout()) {
						//System.out.println("time out Occured");
						sequenceNo = windowBase;
						data.position(windowBase * udpPktSize);
					}
				}
			}
			clientSocket.close();
			long endCount = System.currentTimeMillis();
			System.out.println("time elasped: " + (endCount - startCount));
		} catch (BufferUnderflowException e) {
			System.out.println("Error:" + e);
		}
	}

	// To receive the data from datagram socket
	public static byte[] rdt_rcv(int pktSize) {
		try {
			// Initialize byte array for receiving data
			byte[] packet = new byte[pktSize];
			// Read the data to the byte array from the socket
			DatagramPacket receivePacket = new DatagramPacket(packet, packet.length);
			try {
				clientSocket.setSoTimeout(receiveTimer);
				clientSocket.receive(receivePacket);
			} catch (SocketTimeoutException e) {
				return null;
			}
			/*
			 * // Get Ip address of the client IPAddress =
			 * receivePacket.getAddress(); // Get the Port number of the client
			 * port = receivePacket.getPort();
			 */
			return packet;
		} catch (IOException e) {
			System.out.println("Error:" + e);
		}
		return null;
	}

	public static void main(String args[]) throws Exception {
		try {
			// Open a Input file stream
			FileInputStream fIn = new FileInputStream("ex3.BMP");
			// Obtain a Input channel to the file
			FileChannel fChanIn = fIn.getChannel();
			// Allocate buffer
			ByteBuffer fBufIn = ByteBuffer.allocate((int) fChanIn.size());
			// Read the data form Input channel to buffer and
			// close channel and file stream
			fChanIn.read(fBufIn);
			fChanIn.close();
			fIn.close();
			// Rewind the buffer so it is ready read
			fBufIn.rewind();
			// Send the data
			rdt_send(fBufIn);
		} catch (IOException e) {
			System.out.println("Error:" + e);
		}
	}
}