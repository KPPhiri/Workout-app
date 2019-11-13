# server.py
import sys
import socket
import random
import threading 
from collections import Counter




def create_game_packet(msg_flag, word_length, num_incorrect, data, incorrect_guesses):
	byte_arr = bytearray([msg_flag, word_length, num_incorrect])
	byte_arr.extend(data.encode())
	for guess in incorrect_guesses:
		byte_arr.extend(guess.encode())
	return byte_arr

def create_game_message_packet(msg_flag, data):
	byte_arr = bytearray([msg_flag])
	byte_arr.extend(data.encode())
	return byte_arr


def find_indexes(guess, guess_word, current_guess):
	indexes = []

	for indx, char in enumerate(guess_word):
		if char == guess and current_guess[indx] == '_':
			indexes.append(indx)
	return indexes

def create_single_game_simulation(connections, connection, guess_words):
	guess_word = random.choice(guess_words)
	current_guess = '_'* len(guess_word)
	num_incorrect = 0
	incorrect_guesses = []
	game_over = False
	print("Starting game, random word chosen: {}".format(guess_word))
	
	connections.add(connection)
	game_packet = create_game_packet(0, len(current_guess), num_incorrect, current_guess, incorrect_guesses)
	connection.send(game_packet)

	while not game_over: 
		response=connection.recv(2000)
		recv_msglen = response[0]
		recv_guess = "".join( chr(x) for x in bytearray(response[1:]))


		if recv_msglen == 1:
			guess_indexes = find_indexes(recv_guess, guess_word, current_guess)

			if len(guess_indexes) == 0:
				num_incorrect += 1
				incorrect_guesses.append(recv_guess)
			else:
				current_guess_arr = list(current_guess)
				for indx in guess_indexes:
					current_guess_arr[indx] = recv_guess

				current_guess = ''.join(current_guess_arr)


		game_packet = create_game_packet(0, len(current_guess), num_incorrect, current_guess, incorrect_guesses)
		connection.send(game_packet)

		if Counter(current_guess)['_'] == 0:
			game_packet = create_game_message_packet(8, "You win!")
			game_over = True
		elif num_incorrect >= 6:
			game_packet = create_game_message_packet(9, "You Lost!")
			game_over = True

		if game_over:
			connections.remove(connection)
			connection.send(game_packet)
			# print('closing connection')
			connection.close() 


def create_multiplayer_game_simulation(connections, connection1, connection2, guess_words):
	guess_word = random.choice(guess_words)
	current_guess = '_'* len(guess_word)
	num_incorrect = 0
	incorrect_guesses = []
	game_over = False
	print("Starting game, random word chosen: {}".format(guess_word))
	
	# connections.add(connection1)
	# connections.add(connection2)
	starting_msg1 = create_game_message_packet(14, "Game Starting Player 1!")
	starting_msg2 = create_game_message_packet(14, "Game Starting Player 2!")
	connection1.send(starting_msg1)
	connection2.send(starting_msg2)

	connection = connection1

	while not game_over: 
		game_packet = create_game_packet(0, len(current_guess), num_incorrect, current_guess, incorrect_guesses)
		connection.send(game_packet)
		response=connection.recv(2000)
		recv_msglen = response[0]
		recv_guess = "".join( chr(x) for x in bytearray(response[1:]))


		if recv_msglen == 1:
			guess_indexes = find_indexes(recv_guess, guess_word, current_guess)

			if len(guess_indexes) == 0:
				num_incorrect += 1
				incorrect_guesses.append(recv_guess)
			else:
				current_guess_arr = list(current_guess)
				for indx in guess_indexes:
					current_guess_arr[indx] = recv_guess

				current_guess = ''.join(current_guess_arr)


		if Counter(current_guess)['_'] == 0:
			game_packet = create_game_message_packet(8, "You win!")
			game_over = True
		elif num_incorrect >= 6:
			game_packet = create_game_message_packet(9, "You Lost!")
			game_over = True

		if game_over:
			# connections.remove(connection1)
			# connections.remove(connection2)
			connection1.send(game_packet)
			connection2.send(game_packet)

			connection1.close() 
			connection2.close() 

		if (connection == connection2):
			connection = connection1
		else:
			connection = connection2



#Server socket setup
#AF_INET= ipv4, SOCK_STREAM=TCP
sock = socket.socket()
if(len(sys.argv) == 3):
	ip = sys.argv[1]
	port = int(sys.argv[2])
else: 
	ip = ''
	port = 9012

sock.bind((ip, port))
sock.listen(5)

#Getting guess words
file = open('../word.txt')
guess_words = file.read().split('\n')
file.close()
connections = set()
waiting_room = []

while True: 
	# Establish connection with client.
	connection, addr = sock.accept()  
	response=connection.recv(2000)
	message = response[1:].decode()

	if len(connections) >= 3:
		msg = "server-overload"
		game_packet = create_game_message_packet(len(msg), msg)
		connection.send(game_packet)
		connection.close() 
	elif message == 'n': 	
		threading.Thread(target = create_single_game_simulation, kwargs={'connections':connections , 'connection':connection, 'guess_words': guess_words}).start()
	elif message == 'y':
		if len(waiting_room) == 1:
			connection1 = waiting_room[0]
			connection2 = connection

			threading.Thread(target = create_multiplayer_game_simulation, kwargs={'connections':connections , 'connection1':connection1, 'connection2':connection2, 'guess_words': guess_words}).start()
			waiting_room.pop()
		else:
			waiting_room.append(connection)
			waiting_msgs = create_game_message_packet(25, "Waiting for other player!")
			connection.send(waiting_msgs)




	# Close the connection with the client 
	# connection.close() 


