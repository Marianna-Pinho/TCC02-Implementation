SERVER_HOST = "200.129.38.153"

LOGIN = ""
PASSWORD = ""
COLLECTOR_ID = ""

# Define a location to local database
DATABASE_DIR = "data"

# Define a location to log files
LOG_DIR = "log"

# Override with the collector adress, normaly located in /dev/ttyACM...
COLLECTOR_ADDRESS = '/dev/ttyACM0'

# 10 seconds. After this time, the not sent messages will be deleteds
MAX_MESSAGE_AGE = 60 * 1000

DATA_OUTPUT_ENABLED = False
DATA_OUTPUT_HOST = "127.0.0.1"
DATA_OUTPUT_PORT = 30003

LOCAL_DATA_ENABLED = False

try:
    import config
    attrs = [item for item in dir(config) if not item.startswith("__")]

    for attr in attrs:
        try:
            globals()[attr] = getattr(config, attr)
        except Exception as err:
            print ("Erro:", err)
except:
    pass