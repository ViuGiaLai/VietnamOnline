CXX = g++
CXXFLAGS = -std=c++17 -Wall -Ishared/include -Iclient/prototype/include

SERVER_CFLAGS = -std=c++17 -Wall -Ishared/include \
	-Iserver/common/include -Iserver/gateway/include \
	-Iserver/auth-service/include -Iserver/profile-service/include \
	-Iserver/world-service/include -Iserver/movement-sync-service/include \
	-Iserver/chat-service/include -Iserver/poi-service/include \
	-Iserver/quest-service/include -Iserver/vehicle-service/include

PROTOTYPE_SRC = client/prototype/src/main.cpp client/prototype/src/GameEngine.cpp client/prototype/src/AuthManager.cpp shared/src/EnvConfig.cpp shared/src/HttpClient.cpp shared/src/SupabaseClient.cpp shared/src/RedisClient.cpp shared/src/WeatherService.cpp shared/src/OsmClient.cpp
PROTOTYPE_OBJ = $(patsubst client/prototype/src/%.cpp,build/%.o,$(filter client/prototype/src/%,$(PROTOTYPE_SRC))) build/shared/EnvConfig.o build/shared/HttpClient.o build/shared/SupabaseClient.o build/shared/RedisClient.o build/shared/WeatherService.o build/shared/OsmClient.o
PROTOTYPE_BIN = build/vno_prototype

SERVER_SRC = server/src/main.cpp server/common/src/ServiceRegistry.cpp shared/src/EnvConfig.cpp \
	server/gateway/src/GatewayService.cpp server/gateway/src/WebSocketServer.cpp server/gateway/src/SessionManager.cpp server/gateway/src/ClientSession.cpp server/auth-service/src/AuthService.cpp \
	server/profile-service/src/ProfileService.cpp server/world-service/src/WorldService.cpp \
	server/movement-sync-service/src/MovementSyncService.cpp server/chat-service/src/ChatService.cpp \
	server/poi-service/src/PoiService.cpp server/quest-service/src/QuestService.cpp \
	server/vehicle-service/src/VehicleService.cpp

SERVER_OBJ = $(patsubst server/%,build/server/%,$(filter server/%,$(SERVER_SRC)))
SERVER_OBJ := $(SERVER_OBJ:.cpp=.o) build/shared/EnvConfig.o build/shared/HttpClient.o build/shared/SupabaseClient.o build/shared/RedisClient.o build/shared/WeatherService.o build/shared/OsmClient.o
SERVER_BIN = build/vno_server
TEST_BIN = build/tests_validation

.PHONY: all prototype server run test clean

all: prototype server test

prototype: $(PROTOTYPE_BIN)

server: $(SERVER_BIN)

test: $(TEST_BIN)
	./$(TEST_BIN)

$(PROTOTYPE_BIN): $(PROTOTYPE_OBJ)
	@mkdir -p build data/saves
	$(CXX) $(CXXFLAGS) -o $@ $(PROTOTYPE_OBJ)

build/%.o: client/prototype/src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/shared/%.o: shared/src/%.cpp
	@mkdir -p build/shared
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SERVER_BIN): $(SERVER_OBJ)
	$(CXX) $(SERVER_CFLAGS) -o $@ $(SERVER_OBJ)

build/server/%.o: server/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(SERVER_CFLAGS) -c $< -o $@

$(TEST_BIN): tests/validation/test_validation.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $@ $<

run: prototype
	./$(PROTOTYPE_BIN)

clean:
	rm -rf build
