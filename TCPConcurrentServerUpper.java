import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.*;
import java.net.*;
import java.util.*;

public class TCPConcurrentServerUpper {
    public static void main(String[] args) throws IOException {
        Selector selector = Selector.open();
        ServerSocketChannel server = ServerSocketChannel.open();
        server.bind(new InetSocketAddress(5000));
        server.configureBlocking(false);
        server.register(selector, SelectionKey.OP_ACCEPT);
	System.out.println("Server running....");
        while (true) {
            selector.select();
            for (Iterator<SelectionKey> it = selector.selectedKeys().iterator(); it.hasNext(); ) {
                SelectionKey key = it.next(); it.remove();
                if (key.isAcceptable()) {
                    SocketChannel client = server.accept();
                    client.configureBlocking(false);
                    client.register(selector, SelectionKey.OP_READ);
                } else if (key.isReadable()) {
                    SocketChannel client = (SocketChannel) key.channel();
                    ByteBuffer buffer = ByteBuffer.allocate(1024);
                    if (client.read(buffer) == -1) { client.close(); continue; }
                    buffer.flip();
                    client.write(ByteBuffer.wrap(new String(buffer.array(), 0, buffer.limit()).toUpperCase().getBytes()));
                }
            }
        }
    }
}
