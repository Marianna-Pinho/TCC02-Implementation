import time

#Monography tests class

class SystemStats():
    
    collector = 1
    hardware_p = 1
    cpu_file = "stats_dir/cpu_usage_python.csv"
    mem_file = "stats_dir/mem_usage_python.csv"
    dectime_file = "stats_dir/decoding_time_python.csv"
    all_msg_file = "stats_dir/all_received_msgs_python.csv"
    adsb_msg_file = "stats_dir/adsb_received_msgs_python.csv"
    decoded_msg_file = "stats_dir/decoded_msgs_python.csv"
    not_decoded_adsb_msg_file = "stats_dir/not_decoded_adsb_msgs_python.csv"
    stats_timeout = 5

    def CPU_usage(self):
        with open("/proc/stat", "r") as fcpu:
            line = fcpu.readline()
            total = 0
            lineL = line.split(" ")[2:]
            for l in lineL:
                total = total + int(l)
            
            pidle = float(lineL[3])/total
            fcpu.close()
            return 1.0-pidle
    
    def MEM_usage(self):
        with open("/proc/meminfo", "r") as fmem:
            lines = fmem.readlines()[:5] #4 na orange pi, 5 no pc
            #totalMemory
            memTotal = int(lines[0].split(" ")[-2])
            
            #MemFree
            totalFree = int(lines[1].split(" ")[-2])
            for l in lines[3:]: #2 na orange pi, 3 no pc
                lineL = l.split(" ")
                totalFree = totalFree + int(lineL[-2])
            
            fmem.close()
            return memTotal - totalFree
    
    def getCurrentTimeMilli(self):
        timeM = int(round(time.time() * 1000))
        return timeM
   
    def saveDecodingTime(self, decTime):
        with open(self.dectime_file,"a") as fdec:
            fdec.write("{},{},{},{}\n".format(self.collector, self.hardware_p, decTime, self.getCurrentTimeMilli()))
            fdec.close()
    
    def saveReceivedMessage(self, msg, path):
        with open(path, "a") as fmsg:
            fmsg.write("{},{},{},{}\n".format(self.collector, self.hardware_p, msg, self.getCurrentTimeMilli()))
            fmsg.close()

    def saveSystemStats(self):
        while(1):
            try:
                fcpu = open(self.cpu_file, "a")
            except IOError:
                print("It was not possible to open the fcpu file!")
                continue
            try:
                fmem = open(self.mem_file, "a")
            except IOError:
                print("It was not possible to open the fmem file!")
                continue

            now = self.getCurrentTimeMilli()
            fcpu.write("{},{},{},{}\n".format(self.collector, self.hardware_p, self.CPU_usage(), now))
            fmem.write("{},{},{},{}\n".format(self.collector, self.hardware_p, self.MEM_usage(), now))

            fcpu.close()
            fmem.close()

            time.sleep(self.stats_timeout)