
def get_instance_and_k(line):
    line = line.replace("instance ", "")
    line = line.replace("with k=", "")
    ins, k = line.split()
    str_ins = "8-5-" + ins
    return str_ins, int(ins), k

def get_expanded(line):
    line = line.replace("Expanded until last jump: ", "")
    line = line.replace(" state(s).", "")
    return line

def get_search_time(line):
    line = line.replace("Search time: ", "")
    return line

def get_total_time(line):
    line = line.replace("Total time: ", "")
    return line

results = open("../results/2c.txt", "r")
index = 0
for line in results:
    line = line.replace("\n", "")
    index += 1
    if index == 1:
        ins, x, k = get_instance_and_k(line)
    if index == 4:
        exp_builtin = get_expanded(line)
    if index == 5:
        search_builtin = get_search_time(line)
    if index == 6:
        total_builtin = get_total_time(line)
    if index == 8:
        exp_planopt = get_expanded(line)
    if index == 9:
        search_planopt = get_search_time(line)
    if index == 10:
        total_planopt = get_total_time(line)
    if index == 11:
        print(ins + ',' + k + ',' + search_builtin + ',' + total_builtin + ',' + exp_builtin + ',' + search_planopt + ',' + total_planopt + ',' + exp_planopt)
        index = 0
    