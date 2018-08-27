
instance = input("Ingrese el nombre de la instancia\n")

with open('instances_WEN2/'+instance+".txt") as f_instance:
    f_transformed = open('instances_WEN/transformed_'+instance+'.txt', "w")
    f_tw = open('instances_WEN2/' + instance + '_tw.txt',"r")

    read_data = f_instance.readline()
    token = read_data.split(" ")
    f_transformed.write(" NUMBER OF REQUESTS\n")
    requests =int(token[0])
    f_transformed.write(" "+token[0]+"\n\n")
    f_transformed.write(" NUMBER OF CROSSDOCKS\n")
    f_transformed.write("  1\n\n")
    f_transformed.write(" VEHICLE CAPACITY          VEHICLE SPEED\n")
    f_transformed.write("       "+token[1]+"                        "+token[4]+"\n\n")
    f_transformed.write(" UNIT_TIME_FOR_PALLET      FIXED_TIME_PREPARATION\n")
    f_transformed.write("   "+token[6].rstrip('\n')+"                             "+token[5]+"\n")
    f_transformed.write(" REQUEST      SUPPLIER                                            CUSTOMER\n")
    f_transformed.write(" CUST NO.     XCOORD.     YCOORD.     READY TIME     DUE DATE     XCOORD.     YCOORD.     READY TIME     DUE DATE     DEMAND\n")

    read_data = f_instance.readline()
    token = read_data.split(" ")
    token = list(filter(None, token))
    f_transformed.write("    "+token[0]+"          "+token[3]+"     "+token[4]+"         0          10000        "+token[1]+"      "+token[2]+"         0           10000         "+token[5])

    for i in range(requests):
        read_data =f_instance.readline()
        read_tw =f_tw.readline()
        token_data = read_data.split(" ")
        token_data = list(filter(None, token_data))
        token_tw = read_tw.split(" ")
        token_tw = list(filter(None, token_tw))
        f_transformed.write("    " + token_data[0] + "          " + token_data[3] + "     " + token_data[4] + "        "+token_tw[2]+"         "+token_tw[3].rstrip('\n')+"          " + token_data[1] + "      " + token_data[2] + "        "+token_tw[0]+"          "+token_tw[1]+"           " + token_data[5])








