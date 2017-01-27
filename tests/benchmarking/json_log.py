import json, sys

                ########################### LOGFILES #########################
sChunkSep = '---'           ## A single line like this separates chunks in a logfile

##############################################################################################
###################### Logfile I/O #####################
##############################################################################################
## PROBLEM: JSON does not allow appending.
## WORKAROUND: separate [JSON] chunks by '---', inspired by https://github.com/pvorb/jsml.
## Writing an arbitrary chunk: ensure end-of-line before separator
def writeLogChunk( wf, ch ):
    ss = ch.__str__()
    wf.write( ss )
    if 0<len(ss) and not ss.endswith( '\n' ):
        wf.write( '\n' )
    wf.write( sChunkSep + '\n' )
    
## Reading an arbitrary log chunk as a string
def readLogChunk( rf ):
    ch = ''
    while True:
        line = rf.readline()
        if 0==len(line) or sChunkSep==line.strip():
            return ch
        ch += line
        
## Would return empty from empty chunk even when the original file continues
def readLogJSONChunk( rf ):
    chJ = None
    ch = readLogChunk( rf )
    if 0<len(ch):
        try:
            chJ = json.loads( ch )
        except:
            print("\n   WARNING: failed to parse JSON chunk '", ch,
                  "' in file pos ", rf.tell(), ".  SYSINFO:  ", sys.exc_info()[0]
                  , sep='')
            raise
    return chJ
            
