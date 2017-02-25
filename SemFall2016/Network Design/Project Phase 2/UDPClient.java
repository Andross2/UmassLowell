import java.io.*;
import java.nio.*;
import java.nio.channels.*;
import java.net.*;

class UDPClient {
	//UDP Packet data size is limited to 1KB
	public static int udpPktSize = 1024;
	
	//To frame a packet for given data
	public static byte[] make_pkt(byte[] data) {
		// Initialize byte array for framing packet
		byte[] packet = new byte[data.length];
		packet = data;
		return packet;
	}
	//To send the through datagram socket
	public static void udt_send(byte[] sendData) {
		try {
			// Open Client Socket
			DatagramSocket clientSocket = new DatagramSocket();
			// Get Ip address of the host
			InetAddress IPAddress = InetAddress.getByName("localhost");
			// Send the data from byte buffer through the socket
			DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, 9876);
			clientSocket.send(sendPacket);
			// Close the Client Socket
			clientSocket.close();
		} catch (IOException e) {
			System.out.println("Error:" + e);
		}
	}
	//To break the file into 1KB and send 
	public static void rdt_send(ByteBuffer data) {
		try {
			byte[] packetData = new byte[udpPktSize];
			// Read bytes from byte buffer to the byte array
			while(true) {
				data.get(packetData);
				byte[] packet = make_pkt(packetData);
				udt_send(packet);
			}
		} catch (BufferUnderflowException e) {
			byte[] packetDataLast = new byte[data.remaining()];
			data.get(packetDataLast);
			byte[] packetLast = make_pkt(packetDataLast);
			udt_send(packetLast);
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
			//Send the data 
			rdt_send(fBufIn);
		} catch (IOException e) {
			System.out.println("Error:" + e);
		}
	}
}