# client.py
import sys
import socket                
from collections import Counter

def create_game_packet(msg_flag, data):
	byte_arr = bytearray([msg_flag])
	byte_arr.extend(data.encode())
	return byte_arr

def single_player_simulation(sock):
	guesses = []
	prev_guess = None
	guess = None

	while True:
		response = sock.recv(1024)
		recv_msgflag = response[0]
		recv_num_incorrect = response[2]

		if recv_msgflag != 0:
			message = "".join(chr(x) for x in bytearray(response[1:]))
			print(message)
			return 
		
		if recv_num_incorrect != 0:
			incorrect_guesses = " ".join(chr(x) for x in bytearray(response[-recv_num_incorrect:]))
			current_guess = " ".join( chr(x) for x in bytearray(response[3:-recv_num_incorrect]))
		else:
			incorrect_guesses = ""
			current_guess = " ".join( chr(x) for x in bytearray(response[3:]))

		prev_guess = current_guess

		print(current_guess + '\nIncorrect Guesses: ' + incorrect_guesses)

		if Counter(current_guess)['_'] == 0 or recv_num_incorrect == 6:
			continue
		
		keep_guessing = True

		while keep_guessing:
			guess = input("\nLetter to guess: ").lower()
			if len(guess) != 1 or not guess.isalpha():
				print('Error! Please guess one letter.')
			elif guess in guesses:
				print('Error! Letter a has been guessed before, please guess another letter.')
			else:
				keep_guessing = False
				guesses.append(guess)

		guess_packet = create_game_packet(1, guess)
		sock.send(guess_packet)
		
def multi_player_simulation(sock):
	guesses = []
	prev_guess = None
	guess = None
	message = ""

	response = sock.recv(1024)
	message = "".join(chr(x) for x in bytearray(response[1:]))

	if message == "Waiting for other player!":
		print(message)
		response = sock.recv(1024)
		message = "".join(chr(x) for x in bytearray(response[1:]))

	print(message)
	player_num = int(message[-2:-1])
	opp_player_num = 2

	if player_num == 2:
		opp_player_num = 1
		print("\nWaiting on Player {} ...".format(opp_player_num))


	while True:
		response = sock.recv(1024)
		recv_msgflag = response[0]
		recv_num_incorrect = response[2]
		
		if recv_msgflag != 0:
			message = "".join(chr(x) for x in bytearray(response[1:]))
			print(message)
			return 
		
		if recv_num_incorrect != 0:
			incorrect_guesses = " ".join(chr(x) for x in bytearray(response[-recv_num_incorrect:]))
			current_guess = " ".join( chr(x) for x in bytearray(response[3:-recv_num_incorrect]))
		else:
			incorrect_guesses = ""
			current_guess = " ".join( chr(x) for x in bytearray(response[3:]))

		prev_guess = current_guess

		print("\nYour Turn!\n" + current_guess + '\nIncorrect Guesses: ' + incorrect_guesses)

		if Counter(current_guess)['_'] == 0 or recv_num_incorrect == 6:
			continue
		
		keep_guessing = True

		while keep_guessing:
			guess = input("\nLetter to guess: ").lower()
			if len(guess) != 1 or not guess.isalpha():
				print('Error! Please guess one letter.')
			elif guess in guesses:
				print('Error! Letter a has been guessed before, please guess another letter.')
			else:
				keep_guessing = False
				guesses.append(guess)

		guess_packet = create_game_packet(1, guess)
		sock.send(guess_packet)
		print("Waiting on Player {} ...".format(opp_player_num))


if __name__ == '__main__':
	start_game = ''
	while start_game != "y" and start_game != "n":
		start_game = input("Ready to start game? (y/n):")
	if start_game == 'y':
		multi_mode = input("Two Player? (y/n):")
		while multi_mode != 'y' and multi_mode != 'n':
			multi_mode = input("Two Player? (y/n):")



		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

		if(len(sys.argv) == 3):
			ip = sys.argv[1]
			port = int(sys.argv[2])
		else:
			ip = '127.0.0.1'
			port = 9012

		sock.connect((ip, port))
		start_packet = create_game_packet(1, multi_mode)
		sock.send(start_packet)

		if multi_mode == 'y':
			multi_player_simulation(sock)
		else:
			single_player_simulation(sock)
