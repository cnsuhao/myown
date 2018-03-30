import json

class Configure:
    def __init__( self, config_name):
        with open(config_name) as config:
            self.__data = json.load(config)

    def prepare(self):
        for key in self.__data :
            value = self.__data[key];
            self.__data[key] = self.__prepare_value(value, self.__data );

    def get_data(self):
        return self.__data;

    def __prepare_value(self, value, data):
        if isinstance( value, (str, unicode)):
            index = value.find('{@', 0);
            while index != -1:
                end = value.find('}', index);
                if end == -1:
                    raise Exception("config format error, not find variable end flag at '%s'" %(value) );
                var=value[index:end+1];
                key=var[2 : len(var) -1 ];
                value = value.replace(var, str( data[key]) );
                index = value.find( '{@', index );
        return value;

    def dump(self):
        for key in self.__data:
             print(key + ":" + str(self.__data[key]));

if __name__ == "__main__":
    config = Configure("./cfg/developer.json");
    config.prepare();
    config.dump();