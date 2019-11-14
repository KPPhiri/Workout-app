# Contents for Python Makefile 
PY = python3

all: server client

# make sure TAs can make server/client/clean
server:
client:
clean:

run-server: server.py
	$(PY) $< $(filter-out $@,$(MAKECMDGOALS))
run-client: client.py
	$(PY) $< $(filter-out $@,$(MAKECMDGOALS))