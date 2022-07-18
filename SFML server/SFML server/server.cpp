#include<SFML/Audio.hpp>
#include<SFML/Audio/Sound.hpp>
#include<SFML/Audio/SoundBufferRecorder.hpp>
#include<SFML/Network.hpp>
#include<SFML/Network/TcpListener.hpp>
#include<iostream>

const sf::Uint8 clientAudioData = 1;
const sf::Uint8 clientEndOfStream = 2;




int main() 
{

	//Initialize socket
	//sf::IpAddress ip = sf::IpAddress::getPublicAddress();
	sf::TcpSocket remoteSocket;
	sf::TcpListener listener1;

	if (listener1.listen(8080) != sf::Socket::Done)
	{
		std::cout << "Server sfml is not listening" << std::endl;
	}
	else
	{
		std::cout << "Server sfml is listening...." << std::endl;
	}

	if (listener1.accept(remoteSocket) != sf::Socket::Done)
	{
		std::cout << "SFML server didn't accept client" << std::endl;
	}
	else
	{
		std::cout << "Server sfml accepted client" << std::endl;
	}


	//Initialize Recorder
	if (!sf::SoundBufferRecorder::isAvailable())
	{
		std::cout << "Microphone is not available" << std::endl;

	}



	do
	{
		int flag1 = 0, flag2 = 1;
		sf::Packet packet, packet2, packetflag ;
		const sf::Int16* samples;
		sf::SoundBuffer buffer, b1;
		sf::Sound sound;

		if (flag2 == 1)
		{

			flag1 = 0;
			flag2 = 0;

			// Create the recorder
			sf::SoundBufferRecorder recorder;

			std::cout << "Please speak up" << std::endl;


			if (!recorder.start(44100))
			{
				std::cerr << "Failed to start recorder" << std::endl;
				return 0;
			}


			//std::cout << "Please speak up" << std::endl;

			sf::sleep(sf::milliseconds(6000));

			recorder.stop();

			buffer = recorder.getBuffer();


			samples = buffer.getSamples();
			std::size_t sampleCount = buffer.getSampleCount();


			packet << clientAudioData;
			packet.append(samples, sampleCount * sizeof(sf::Int16));

			// Send the audio packet to the client
			if (remoteSocket.send(packet) != sf::Socket::Done)
			{

				std::cerr << "Failed to send end-of-stream packet" << std::endl;
			}

			//Tell reciever about end of packet

			flag1 = 2;
			packetflag << flag1;

			if (remoteSocket.send(packetflag) != sf::Socket::Done)
			{

				std::cerr << "Failed to send end-of-stream packet" << std::endl;
			}

		}

		else
		{
			flag2 = 2;
		}





		if (flag2 == 2)
		{
			//Recieve client audio
			sf::Uint8 id;
			std::vector<sf::Int16> m_samples;

			if (remoteSocket.receive(packet2) != sf::Socket::Done)
				std::cout << "Audio wasn't recieved" << std::endl;

			packet2 >> id;

			if (id == 1 || id == 2)
			{


				// Extract audio samples from the packet, and append it to our samples buffer
				std::size_t sampleCount2 = (packet2.getDataSize() - 1) / sizeof(sf::Int16);


				std::size_t oldSize = m_samples.size();
				m_samples.resize(oldSize + sampleCount2);
				std::memcpy(&(m_samples[oldSize]), static_cast<const char*>(packet.getData()) + 1, sampleCount2 * sizeof(sf::Int16));



				b1.loadFromSamples(m_samples.data(), sampleCount2, 1, 45000);

				std::cout << "Recieving Audio" << std::endl;
				sound.setBuffer(b1);
				sound.setVolume(100);
				sound.play();
				sf::sleep(sf::microseconds(b1.getSampleCount() * 22));
				sound.stop();


			}
			else
			{
				// Something's wrong...
				std::cout << "Invalid packet received..." << std::endl;

			}

		}

		else
		{
			flag2 = 1;

		}

		
		




	} while (1);
	
	
	//std::cin.ignore(10000, '\n')

}
