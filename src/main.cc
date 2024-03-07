#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

	// Disable output buffering
	setbuf(stdout, NULL);

	int udp_socket;
	struct sockaddr_in client_address;
	udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(udp_socket == -1) {
		std::cerr << "Socket creation failed: " << strerror(errno) << "...\n";
		return 1;
	}

	int option{1};
	if(setsockopt(udp_socket, SOL_SOCKET, SO_REUSEPORT, &option, sizeof(option)) < 0) {
		std::cerr << "SO_REUSEPORT failed: " << strerror(errno) << '\n';
		return 1;
	}

	sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(2053),
		.sin_addr = {htonl(INADDR_ANY)},
	};

	if(bind(udp_socket, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) != 0) {
		std::cerr << "Bind failed: " << strerror(errno) << '\n';
		return 1;
	}

	int bytes_read;
	char buffer[512];

	socklen_t client_addr_len = sizeof(client_address);

	while(true) {
		// Receive data
		bytes_read = recvfrom(udp_socket, buffer, sizeof(buffer), 0, reinterpret_cast<struct sockaddr*>(&client_address), &client_addr_len);
		if(bytes_read == -1) {
			std::cerr << "Error receiving data\n";
			break;
		}

		buffer[bytes_read] = '\0';
		std::cout << "Received " << bytes_read << " bytes: " << buffer << '\n';

		// Create an empty response
		char response[1] = {'\0'};

		// Send response
		if(sendto(udp_socket, response, sizeof(response), 0, reinterpret_cast<struct sockaddr*>(&client_address), sizeof(client_address)) == -1) {
			std::cerr << "Failed to send response\n";
		}

	}

	close(udp_socket);

	return 0;
}
