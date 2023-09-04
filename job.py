mport requests
import asyncio
from time import sleep
from telegram import Bot
# Configura los parámetros de la API de ThingSpeak
channel_id = '2254949'
read_api_key = 'UFNYFZTLLFVCO7J1'
channel_id2 = '2254950'
read_api_key2 = 'WLLZMRRYNMZ4QQXT'
channel_id3 = '2254951'
read_api_key3 = 'LQ6TI5IKF7GUJHTK'
num_entries = 1  # Número de entradas que deseas obtener
requests.packages.urllib3.disable_warnings()
numero = 0
bot_token = '6522617525:AAFy8qsO1CVgokEl-_WWjwhbNf6dAHnVHRA'
chat_id = '1241584779'  # Replace with your actual chat ID
bot = Bot(token=bot_token)

# URL de la API de lectura de ThingSpeak
bucket = "storage"
org = "Prueba"
token = "gRrFFfn4Lw593L05CiXjK9eIqxbQ9_aqvoQ6LWYOchcXplxX9iRHyaUnmQs87dzeaxMJkhKTgARIwAA6nIh9Fg=="
url = "https://34.174.5.152:8086"

def obtener_datos():
    try:
        url2 = f'https://api.thingspeak.com/channels/{channel_id}/feeds.json?api_key={read_api_key}&results={num_entries}'
        response = requests.get(url2)
        url3 = f'https://api.thingspeak.com/channels/{channel_id2}/feeds.json?api_key={read_api_key2}&results={num_entries}'
        response2 = requests.get(url3)
        url4 = f'https://api.thingspeak.com/channels/{channel_id3}/feeds.json?api_key={read_api_key3}&results={num_entries}'
        response3 = requests.get(url4)
        if response.status_code == 200 and response2.status_code == 200 and response3.status_code == 200:
            data = response.json()
            entry = data['feeds'][0]  # Assuming you're only getting one entry
            print("valores: ", entry)  # Assuming you're only getting one entry
            data2 = response2.json()
            entry2 = data2['feeds'][0]  # Assuming you're only getting one entry
            print("valores: ", entry2)  # Assuming you're only getting one entry
            data3 = response3.json()
            entry3 = data3['feeds'][0]  # Assuming you're only getting one entry
            print("valores: ", entry3)  # Assuming you're only getting one entry
            return entry['field2'], entry['field3'], entry['field4'],entry2['field2'], entry2['field3'], entry2['field4'],entry3['field2'], entry3['field3'], entry3['field4']
        else:
            print(f"Error al obtener los datos. Código de estado: {response.status_code}")
            return None, None, None

    except requests.RequestException as e:
        print(f"Error de conexión: {e}")
        return None, None, None



async def main():
    while numero == 0:
        temperature, humidity, position, TDHTnodo2, TDS1nodo2, Humenodo2, TDHTnodo3, TDS1nodo3, Humenodo3 = obtener_datos()

        if temperature is not None and humidity is not None and position is not None:
            print("Temperatura:", temperature)
            print("Humedad:", humidity)
            print("Posición:", position)
            print("Temperatura2:", TDHTnodo2)
            print("Humedad2:", TDS1nodo2)
            print("Posición2:", Humenodo2)
            print("-----------------------------------")

            mensajet = (
                f"Temperatura: {temperature}\n"
                f"Humedad: {humidity}\n"
                f"Posición: {position}\n"
                f"Temperatura2: {TDHTnodo2}\n"
                f"Humedad2: {TDS1nodo2}\n"
                f"Posición2: {Humenodo2}\n"
                "-----------------------------------"
            )
            line_protocol_data = (
                f"measurement,building=IoT "
                f"Temperatura\\ DHT\\ nodo1={float(temperature)},"
                f"Temperatura\\ DS18B20\\ nodo1={float(humidity)},"
                f"Humedad\\ nodo1={float(position)},"
                f"Temperatura\\ DHT\\ nodo2={float(TDHTnodo2)},"
                f"Temperatura\\ DS18B20\\ nodo2={float(TDS1nodo2)},"
                f"Humedad\\ nodo2={float(Humenodo2)},"
                f"Temperatura\\ DHT\\ nodo3={float(TDHTnodo3)},"
                f"Temperatura\\ DS18B20\\ nodo3={float(TDS1nodo3)},"
                f"Humedad\\ nodo3={float(Humenodo3)}"
            )

            headers = {
                "Authorization": f"Token {token}",
                "bucket": bucket,
                "org": org,
                "Content-Type": "text/plain; charset=utf-8",
            }

            response = requests.post(f"{url}/api/v2/write?bucket={bucket}&org={org}", data=line_protocol_data, headers=headers, verify=False)

            if response.status_code == 204:
                print("Data written successfully to InfluxDB")

                # Send the message to your Telegram bot
                await bot.send_message(chat_id=chat_id, text=mensajet)

            else:
                print(f"Error writing data to InfluxDB. Status code: {response.status_code}")
                await bot.send_message(chat_id=chat_id, text=f"Error writing data to InfluxDB. Status code: {response.status_code}")
        await asyncio.sleep(5)  # Add a delay before fetching new data

# Run the asynchronous event loop
if __name__ == "__main__":
    asyncio.run(main())