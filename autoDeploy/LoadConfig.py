import json
gameConfigName = 'GameConfig.json'
def initConfig():
        with open(gameConfigName) as config:
                config = json.load(config)
                return config;

