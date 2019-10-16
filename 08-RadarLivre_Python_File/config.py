SERVER_HOST = "www.radarlivre.com:80"

LOGIN = "admin3"
PASSWORD = "RLadmin#"
COLLECTOR_ID = "5980d171-73fb-46c1-aea1-e7b5d5c1de60"

# Define a location to local database
DATABASE_DIR = "data"

# Define a location to log files
LOG_DIR = "log"

# Override with the collector adress, normaly located in /dev/ttyACM...
COLLECTOR_ADDRESS = '/dev/ttyACM0'

# 60 seconds. After this time, the not sent messages will be deleteds. Meassured in milliseconds
MAX_MESSAGE_AGE = 60 * 1000

DATA_OUTPUT_ENABLED = False
DATA_OUTPUT_HOST = "127.0.0.1"
DATA_OUTPUT_PORT = 30003

LOCAL_DATA_ENABLED = False
