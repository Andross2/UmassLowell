import java.io.*;
import java.nio.*;
import java.nio.channels.*;
import java.util.Random;
import java.util.concurrent.ConcurrentHashMap;
import java.net.*;

class Receiver extends UDPClient {
	// Receiver non-blocking Timer value
	public int receiveTimer = 25;
	// ACK value
	public static byte ACK = 1;

	Receiver() {
		receiveTimer = 25;
	}

	Receiver(int receiveTimer) {
		this.receiveTimer = receiveTimer;
	}

	// To find the received packet is not corrupted.
	public boolean notcorrupt(byte[] data) {
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
	public int isACK(byte[] data) {
		if (data == null)
			return -1;
		try {
			int seqTmp = 0;
			short chkSumtmp = 0;
			byte ackTmp = 0;
			// convert data array to byte buffer
			ByteBuffer dataBuf = ByteBuffer.wrap(data);
			seqTmp = dataBuf.getInt();
			// System.out.println(seqTmp);
			chkSumtmp = dataBuf.getShort();
			ackTmp = dataBuf.get();
			if ((notcorrupt(data)) && (ackTmp == ACK))
				return seqTmp;
			return -1;
		} catch (Exception e) {
			System.out.println("Error:" + e);
		}
		return -1;
	}

	// To receive the data from datagram socket
	public byte[] rdt_rcv(int pktSize) {
		try {
			// Initialize byte array for receiving data
			byte[] packet = new byte[pktSize];
			// Read the data to the byte array from the socket
			DatagramPacket receivePacket = new DatagramPacket(packet, packet.length);
			try {
				// clientSocket.setSoTimeout(receiveTimer);
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
}

class Sender extends UDPClient {
	// sequence + checksum size
	public static int additionalSize = 6;
	// checksum size is 2 bytes(16 bit) in UDP
	public static int checkSumSize = 2;

	// To calculate checksum
	public short CheckSum(int seqNo, byte[] data) {
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
	public byte[] make_pkt(int seqNo, byte[] data, short checkSum) {
		// Initialize byte array for framing packet
		ByteBuffer packetBuf = ByteBuffer.allocate(data.length + additionalSize);
		//int randTmp = rand.nextInt(100) + 1;
		// System.out.println("rand:"+randTmp);
		//if (randTmp <= percentBitError)
			//packetBuf.putInt(-seqNo);
		//else
			packetBuf.putInt(seqNo);
		// System.out.println(seqNo);
		packetBuf.putShort(checkSum);
		packetBuf.put(data);
		packetBuf.rewind();
		byte[] packet = packetBuf.array();
		return packet;
	}

	// To send the through datagram socket
	public void udt_send(byte[] sendData) {
		try {
			// Get Ip address of the host
			IPAddress = InetAddress.getByName("localhost");
			// Send the data from byte buffer through the socket
			DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, port);
			int randTmp2 = rand2.nextInt(100) + 1;
			if (!(randTmp2 <= percentPacketLoss))
				clientSocket.send(sendPacket);
		} catch (IOException e) {
			System.out.println("Error:" + e);
		}
	}
}

class PacketTimer {
	// starting timer value
	public long startTimerValue = 0;
	// packet timeout value
	public long packetTimeout = 250;

	// To start the timer
	public void start_timer() {
		startTimerValue = System.currentTimeMillis();
	}

	// To stop the timer
	public void stop_timer() {
		startTimerValue = 0;
	}

	// To check for time out
	public boolean timer_timeout() {
		if (Math.abs(System.currentTimeMillis() - startTimerValue) >= packetTimeout)
			return true;
		return false;
	}
}

// To create sender thread
class SenderThread implements Runnable {
	// Selected the percentage of packets to be corrupted--Packet
	public static int percentBitError = 10;
	// random number generator
	public static Random rand = new Random();
	private Thread t;
	int seqNo;
	byte[] dataPacket;
	byte[] corruptedPacket;
	ConcurrentHashMap<Integer, String> mylookup = null;

	SenderThread(int seqNo, byte[] dataPacket, ConcurrentHashMap<Integer, String> mylookup) {
		this.seqNo = seqNo;
		this.dataPacket = dataPacket;
		this.mylookup = mylookup;
	}

	public void run() {
		// packet timeout value
		long SenderpacketTimeout = 25;
		Sender snd = new Sender();
		// Wait for ACK
		while (true) {
			snd.udt_send(dataPacket);
			try {
				Thread.sleep(SenderpacketTimeout);
			} catch (InterruptedException e) {
				System.out.println("Error:" + e);
			}
			if (mylookup.containsKey(seqNo)) {
				return;
			}
		}
	}

	public void start() {
		if (t == null) {
			t = new Thread(this);
			t.start();
		}
	}
}

// To create receive thread
class ReceiverThread implements Runnable {
	// ACK packet size
	public static int ackPacketSize = 7;
	public Thread t;
	ConcurrentHashMap<Integer, String> mylookup = null;

	ReceiverThread(ConcurrentHashMap<Integer, String> mylookup) {
		this.mylookup = mylookup;
	}

	public void run() {
		int recSeqNo;
		Receiver rec = new Receiver();
		while (true) {
			recSeqNo = rec.isACK(rec.rdt_rcv(ackPacketSize));
			if (recSeqNo >= 0)
			{
				if(!mylookup.containsKey(recSeqNo))
				mylookup.put(recSeqNo, "R");
			}
		}
	}

	public void start() {
		if (t == null) {
			t = new Thread(this);
			t.start();
		}
	}

}

class UDPClient {
	// UDP Packet data size is limited to 1KB
	public static int udpPktSize = 1024;
	public static InetAddress IPAddress;
	public static int port = 9876;
	public static DatagramSocket clientSocket;
	public static int sequenceNo = 1;
	// Selected the percentage of packets to be lost--Packet
	public static int percentPacketLoss = 0;
	// random number generator
	public static Random rand2 = new Random();
	// Window Size of Go-Back-N
	public static int windowSize = 50;
	// Window base
	public static int windowBase = 1;
	// Hash table
	public static ConcurrentHashMap<Integer, String> lookup = new ConcurrentHashMap<Integer, String>();

	// To break the file into 1KB and send
	@SuppressWarnings("deprecation")
	private static void rdt_send(ByteBuffer data) {
		Sender senTmp = new Sender();
		SenderThread childThread;
		ReceiverThread RecChildThread;
		long startCount = System.currentTimeMillis();
		try {
			// Open Client Socket
			clientSocket = new DatagramSocket();
		} catch (IOException e) {
			System.out.println("Error:" + e);
		}
		try {
			RecChildThread = new ReceiverThread(lookup);
			RecChildThread.start();
			byte[] packetData = new byte[udpPktSize];
			// Read bytes from byte buffer to the byte array
			while (true) {
				// Send the Window
				if (data.remaining() != 0) {
					while ((sequenceNo >= windowBase) && (sequenceNo < (windowBase + windowSize))) {
						if (data.remaining() >= udpPktSize) {
							data.get(packetData);
							byte[] packet = senTmp.make_pkt(sequenceNo, packetData,
									senTmp.CheckSum(sequenceNo, packetData));
							childThread = new SenderThread(sequenceNo, packet, lookup);
							childThread.start();
						} else {
							byte[] packetDataLast = new byte[data.remaining()];
							data.get(packetDataLast);
							byte[] packetLast = senTmp.make_pkt(sequenceNo, packetDataLast,
									senTmp.CheckSum(sequenceNo, packetDataLast));
							childThread = new SenderThread(sequenceNo, packetLast, lookup);
							childThread.start();
						}
						sequenceNo++;
					}
				} else {
					if (windowBase >= sequenceNo)
						break;
				}
				while (lookup.containsKey(windowBase)) {
					windowBase++;
				}
			}
			RecChildThread.t.stop();
			clientSocket.close();
			long endCount = System.currentTimeMillis();
			System.out.println("time elasped: " + (endCount - startCount));
		} catch (BufferUnderflowException e) {
			System.out.println("Error:" + e);
		}
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