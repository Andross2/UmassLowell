import java.io.*;
import java.nio.*;
import java.nio.channels.*;
import java.net.*;

class UDPServer {
     //UDP Packet data size is limited to 1KB
	public static int udpPktSize = 1024;
	
    //To parse data from given packet
	public static byte[] extract(byte[] packet) {
		// Initialize byte array for extracting data
		byte[] packetData = new byte[packet.length];
		packetData = packet;
		return packetData;
	}

	//To receive the data from datagram socket
	public static byte[] rdt_rcv(int pktSize) {
		try {
			// Open Server Socket
			DatagramSocket serverSocket = new DatagramSocket(9876);
			// Initialize byte array for receiving data
			byte[] packet = new byte[pktSize];
			// Read the data to the byte array from the socket
			DatagramPacket receivePacket = new DatagramPacket(packet, packet.length);
			serverSocket.receive(receivePacket);
			// Get Ip address of the client
			InetAddress IPAddress = receivePacket.getAddress();
			// Get the Port number of the client
			int port = receivePacket.getPort();
			// Close the Server Socket
			serverSocket.close();
			return packet;
		} catch (IOException e) {
			System.out.println("Error:" + e);
		}
		return null;
	}
	
	//To collect all the data and supply to the file channel
	public static ByteBuffer deliver_data() {
		// Allocate Buffer to collect the packets
		ByteBuffer data = ByteBuffer.allocate(50905);
		try {
			// Read the bytes from byte array to the buffer
			while (data.remaining()>=udpPktSize)
				data.put(extract(rdt_rcv(udpPktSize)));
			data.put(extract(rdt_rcv(data.remaining())));
		} catch (BufferOverflowException e) {
			System.out.println("Error:" + e);
		}
		// Rewind the buffer so it is ready to read
		data.rewind();
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
