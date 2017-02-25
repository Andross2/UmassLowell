import java.io.*;
import java.nio.*;
import java.nio.channels.*;
import java.util.Random;
import java.util.concurrent.ConcurrentHashMap;
import java.net.*;

class UDPServer {
	public static int onceThru = 1;
	// ACK value
	public static byte[] ACK = { 0x01 };
	// ACK packet size
	public static int ackPacketSize = 7;
	// UDP Packet data size is limited to 1KB
	public static int udpPktSize = 1024;
	// checksum size is 2 bytes(16 bit) in UDP
	public static int checkSumSize = 2;
	// sequence + checksum size
	public static int additionalSize = 6;
	public static InetAddress IPAddress;
	public static int port;
	public static DatagramSocket serverSocket;

	public static boolean flag = true;
	// To find elapsed time
	public static long startCount = 0;

	// Selected the percentage of packets to be corrupted--ACK
	public static int percentBitError = 0;

	// Selected the percentage of packets to be lost--ACK
	public static int percentPacketLoss = 0;

	// random number generator
	public static Random rand = new Random();

	// random number generator
	public static Random rand2 = new Random();
	// Receiver window size
	public static int RecwindowSize = 50;
	// Receiver window base
	public static int RecwindowBase = 1;
	// Hash table
	public static ConcurrentHashMap<Integer, byte[]> Reclookup = new ConcurrentHashMap<Integer, byte[]>();

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
		return 0;
	}

	// To frame a packet for given data
	public static byte[] make_pkt(int onceThruNo, byte[] data, short checkSum) {
		// Initialize byte array for framing packet
		ByteBuffer packetBuf = ByteBuffer.allocate(data.length + additionalSize);
		int randTmp = rand.nextInt(100) + 1;
		// System.out.println("rand:" + randTmp);
		if (randTmp <= percentBitError)
			packetBuf.putInt(-onceThruNo);
		else
			packetBuf.putInt(onceThruNo);
		packetBuf.putShort(checkSum);
		packetBuf.put(data);
		packetBuf.rewind();
		byte[] packet = packetBuf.array();
		return packet;
	}

	// To send the through datagram socket
	public static void udt_send(byte[] sendData) {
		try {
			// Send the data from byte buffer through the socket
			DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, port);
			serverSocket.send(sendPacket);
		} catch (IOException e) {
			System.out.println("Error:" + e);
		}
	}

	// To parse data from given packet
	public static byte[] extract(byte[] packet) {
		// Initialize byte array for extracting data
		byte[] packetData = new byte[packet.length - additionalSize];
		System.arraycopy(packet, additionalSize, packetData, 0, packetData.length);
		return packetData;
	}

	// To receive the data from datagram socket
	public static byte[] rdt_rcv(int pktSize) {
		try {
			// Initialize byte array for receiving data
			byte[] packet = new byte[pktSize];
			// Read the data to the byte array from the socket
			DatagramPacket receivePacket = new DatagramPacket(packet, packet.length);
			serverSocket.receive(receivePacket);
			// Get Ip address of the client
			IPAddress = receivePacket.getAddress();
			// Get the Port number of the client
			port = receivePacket.getPort();
			if (flag) {
				startCount = System.currentTimeMillis();
				flag = false;
			}
			return packet;
		} catch (IOException e) {
			System.out.println("Error:" + e);
		}
		return null;
	}

	// To collect all the data and supply to the file channel
	public static ByteBuffer deliver_data() {
		try {
			// Open Server Socket
			serverSocket = new DatagramSocket(9876);
		} catch (IOException e) {
			System.out.println("Error:" + e);
		}
		// Allocate Buffer to collect the packets
		ByteBuffer data = ByteBuffer.allocate(6220854);
		try {
			byte[] rawData = null;
			// Read the bytes from byte array to the buffer
			while (data.remaining() != 0) {
				if (data.remaining() >= udpPktSize)
					rawData = rdt_rcv(udpPktSize + additionalSize);
				else
					rawData = rdt_rcv(data.remaining() + additionalSize);

				if (notcorrupt(rawData)) {
					// convert data array to byte buffer
					ByteBuffer dataBuf = ByteBuffer.wrap(rawData);
					dataBuf.rewind();
					int onceThruRec = dataBuf.getInt();
					//System.out.println(onceThruRec);
					if ((onceThruRec >= RecwindowBase) && (onceThruRec < (RecwindowBase + RecwindowSize))) {
						if (!Reclookup.containsKey(onceThruRec)) {
							Reclookup.put(onceThruRec, extract(rawData));
						}
					}
					byte[] packetData = make_pkt(onceThruRec, ACK, CheckSum(onceThruRec, ACK));
					int randTmp2 = rand2.nextInt(100) + 1;
					if (!(randTmp2 <= percentPacketLoss))
					{
						udt_send(packetData);
					}
				}
				while (Reclookup.containsKey(RecwindowBase)) {
					data.put(Reclookup.get(RecwindowBase));
					Reclookup.remove(RecwindowBase);
					RecwindowBase++;
				}
			}
			serverSocket.close();
		} catch (BufferOverflowException e) {
			System.out.println("Error:" + e);
		}
		// Rewind the buffer so it is ready to read
		data.rewind();
		long endCount = System.currentTimeMillis();
		System.out.println("time elasped: " + (endCount - startCount));
		return data;
	}

	public static void main(String args[]) throws Exception {
		try {
			// Open Output file stream
			FileOutputStream fOut = new FileOutputStream("testserver.bmp");
			// Open Output channel to the file
			FileChannel fChanOut = fOut.getChannel();
			// Write the data form buffer to output channel and
			// close channel and file stream
			fChanOut.write(deliver_data());
			fChanOut.close();
			fOut.close();
		} catch (IOException e) {
			System.out.println("Error:" + e);
		}
	}
}
