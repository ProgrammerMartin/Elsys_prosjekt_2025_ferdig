import tcpServer2 as tcp
import createCSV as csv
import time



def run_sequence():
    tcp.send_start_command()                     # 1. Send "start\n" til ESP
    print("Venter på data fra ESP...")

    start_time = time.time()  # Start the timer

    # Fetch MPU data
    raw_data_mpu = tcp.receive_all_mpu_data()
    elapsed_time = time.time() - start_time  # Calculate elapsed time

    if elapsed_time >= 10.0:
        print("MPU data brukte 10 sekunder. Hopper over FFT og Piezo.")
        raw_data_fft = []
        raw_data_piezo = []
    else:
        # Fetch FFT and Piezo data only if time allows
        raw_data_fft = tcp.receive_all_fft_data()
        raw_data_piezo = tcp.receive_all_piezo_data()

    if raw_data_mpu or raw_data_fft or raw_data_piezo:
        csv.archive_old_csv_files()  # 🗃️ Flytt gamle CSV-filer kun hvis vi faktisk fikk ny data

    if raw_data_mpu:
        print("MPU data mottatt. Lagrer til CSV...")
        csv.create_csv_file_mpu()
        csv.write_mpu_csv(raw_data_mpu)
        print("MPU data lagret.")
    else:
        print("Ingen MPU data mottatt.")

    if raw_data_fft:
        print("FFT data mottatt. Lagrer til CSV...")

        csv.create_csv_file_fft()
        csv.write_fft_csv(raw_data_fft)
        print("FFT data lagret.")
    else:
        print("Ingen FFT data mottatt.")

    if raw_data_piezo:
        print("Piezo data mottatt. Lagrer til CSV...")

        csv.create_csv_file_piezo()
        csv.write_piezo_csv(raw_data_piezo)
        print("Piezo data lagret.")
    else:
        print("Ingen piezo data mottatt.")

    # ESP er nå tilbake i ventemodus (klar for nytt startsignal)