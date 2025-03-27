import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.*;
import java.util.Iterator;
import java.util.Set;

public class TCPConcurrentPollServer {
    private static final int PORT = 12345;
    private static final int BUFFER_SIZE = 1024;

    public static void main(String[] args) {
        try {
            // Create a non-blocking server socket channel
            ServerSocketChannel serverSocketChannel = ServerSocketChannel.open();
            serverSocketChannel.bind(new InetSocketAddress(PORT));
            serverSocketChannel.configureBlocking(false); // Non-blocking mode

            // Create a selector to handle multiple clients
            Selector selector = Selector.open();
            serverSocketChannel.register(selector, SelectionKey.OP_ACCEPT);

            System.out.println("TCP Concurrent Server using poll() is running on port " + PORT);

            while (true) {
                // Poll for events
                selector.select(); // Blocks until an event occurs

                Set<SelectionKey> selectedKeys = selector.selectedKeys();
                Iterator<SelectionKey> iterator = selectedKeys.iterator();

                while (iterator.hasNext()) {
                    SelectionKey key = iterator.next();
                    iterator.remove(); // Remove key to avoid processing it again

                    if (key.isAcceptable()) {
                        // Accept new client connection
                        ServerSocketChannel serverChannel = (ServerSocketChannel) key.channel();
                        SocketChannel clientChannel = serverChannel.accept();
                        clientChannel.configureBlocking(false);
                        clientChannel.register(selector, SelectionKey.OP_READ);
                        System.out.println("New client connected: " + clientChannel.getRemoteAddress());
                    } else if (key.isReadable()) {
                        // Read data from a client
                        SocketChannel clientChannel = (SocketChannel) key.channel();
                        ByteBuffer buffer = ByteBuffer.allocate(BUFFER_SIZE);
                        int bytesRead = clientChannel.read(buffer);

                        if (bytesRead == -1) {
                            clientChannel.close();
                            System.out.println("Client disconnected");
                        } else {
                            buffer.flip();
                            String message = new String(buffer.array(), 0, bytesRead);
                            System.out.println("Received: " + message);
                            
                            // Echo the message back to the client
                            buffer.rewind();
                            clientChannel.write(buffer);
                        }
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
