
def try_float( sVal, dft=float("nan") ):
    res = dft
    try:
        res = float( sVal )
    except:
        pass
    return res

def mergeDict( dict1, dict2 ):
    for key in dict2:
        dict1[ key ] = dict2[ key ]
