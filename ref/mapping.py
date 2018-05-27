import argparse
import io, subprocess as sp, sys

def argparser():
    parser = argparse.ArgumentParser("mapping")
    parser.add_argument('--map', type=str, required=True)
    #parser.add_argument('--o', type=str, default='output')
    args = parser.parse_args()
    return args 

if __name__ == "__main__":
    args = argparser()
    map= args.map
   # out= args.o

    #convert = open(map, "rb").read().decode(encoding = 'cp950').encode(encoding = "utf-8" )
    #open("ZhuYin-Big5.map","wb").write(convert)
    convert=sp.check_output(['iconv','-f','BIG5','-t','UTF-8', map])
    convert = convert.decode('utf-8')
    convertion = dict()
    chinese  = list()
    for line in convert.split("\n"):
        if len(line) != 0 :
            chin , tmp = line.split(" ")
            chinese.extend(chin)
            zhu = tmp.split("/")
            zhu_f = [i[0] for i in zhu] 
            for item in zhu_f:
                if item not in convertion:
                    convertion[item] = list(chin)
                else:
                    if chin not in convertion[item]:
                        convertion[item].extend(chin) 
    """
    lines = [line for line in convert]
    print(lines)
    """
    #print (convertion)

    with open("ZhuYin-Big5.map","w" ) as f:
        for zhu,chin in convertion.items():
            f.write(zhu+"\t")
            for item in chin:
                f.write(item+" ")
            f.write("\n") 
        for chin in chinese:
            f.write(chin+"\t"+chin+"\n")

    tobig5 = open("ZhuYin-Big5.map", "rb").read().decode(encoding = 'UTF-8' ).encode(encoding = 'cp950' )
    open("ZhuYin-Big5.map","wb").write(tobig5)        
   
    
