import timeit, re, sys, os
import utils, json_config

## TODO Keyline/value dictionaries: entries == json_config.s_CommentKey are ignored. Make it a parameter

class Output:
    def __init__( self ):
        stdout = ""
        stderr = ""

def on_terminate(proc):
    print("process {} terminated with exit code {}".format(proc, proc.returncode))

##############################################################################################
###################### MZN instance execution + result parsing low-level #####################
##############################################################################################
## runCmdCmdline using system(). Actually used for checking as well.
## run the specified shell command string and return the time.
## can add ulimit etc.
## s1, s2: filenames for strout, stderr
## dictCmd: commands for Win and non-Win
## meml: list of 2 values, soft & hard limits as N bytes
def runCmdCmdline( s_Cmd, s1, s2, dictCmd, timeo, bVerbose=False, meml=None ):
    tm = timeit.default_timer()
    setCmd = dictCmd["windows"] if "win" in sys.platform and "cygwin" not in sys.platform else dictCmd["non-windows"]
    sCmd = setCmd["runVerbose" if bVerbose else "runSilent"].format(meml[1], timeo, s_Cmd, s1, s2)
    print( "\n  RUNNING:", sCmd )
    os.system(sCmd)
    tm = timeit.default_timer() - tm
    return tm


## runCmd using psutils. Actually should be used for checking as well.
## run the specified shell command string and return the popen result.
## TODO catch intermediate solutions if needed
## meml: list of 2 values, soft & hard limits as N bytes
def runCmd( s_Cmd, b_Shell=False, timeo=None, meml=None ):
    import psutil, shlex, subprocess, resource
    if b_Shell:
        l_Cmd = s_Cmd
    else:
        l_Cmd = shlex.split( s_Cmd )
    tm = timeit.default_timer()
    ################# In the following, the subprocess.RUN method fails to kill shell calls on Linux
    #try:
    #    completed = subprocess.run( l_Cmd, shell=b_Shell,
    #      universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=timeo )
    #except subprocess.TimeoutExpired as te:
    #    completed = te
    ################# Using psutils
    proc = psutil.Popen(l_Cmd, shell=b_Shell,
          universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if None!=meml:
        if hasattr( psutil, 'RLIMIT_AS' ):  ## TODO move into preexec_fn for parallel tests?
            proc.rlimit( resource.RLIMIT_AS, ( meml[0]*1000, meml[1]*1000 ) )
        else:
            print( "  ... but the OS doesn't support RLIMIT_AS." )
    completed = Output()
    try:
        completed.stdout, completed.stderr = proc.communicate(timeout=timeo)
    except subprocess.TimeoutExpired:
        print ( " soft_kill. ", end='' )
        procs = psutil.Process().children(recursive=True)
        for p in procs:
            p.terminate()
        try:
            completed.stdout, completed.stderr = proc.communicate(timeout=1)
        except subprocess.TimeoutExpired as te:
            print ( " hard_kill. ", end='' )
            procs = psutil.Process().children(recursive=True)
            for p in procs:
                p.kill()
            completed.stdout, completed.stderr = proc.communicate()
    ### Clean up: (does psutil.Process.communicate() wait for all descendants?)  -----------------------------------
    procs = psutil.Process().children(recursive=True)
    for p in procs:
        p.kill()
    ### OR: even Queue?
#    with psutil.Popen(["ifconfig"], stdout=subprocess.PIPE) as proc:
#        log.write(proc.stdout.read())
#
#    procs = psutil.Process().children()
#    for p in procs:
#        p.terminate()
#    gone, still_alive = psutil.wait_procs(procs, timeout=3, callback=on_terminate)
#    for p in still_alive:
#        p.kill()
    tm = timeit.default_timer() - tm
    return completed, tm
    
def parseStderr( f, result, mapKL, mapKV ):
#        result["ProbSense"] = None
#        result["TimeFlt"] = None
    for line in f:
        line = line.strip()
        checkKeylines( line, mapKL, result )
        checkKeyvalues( line, mapKV, result )
        
## Puts feasible solutions into solList if it's not None
def parseStdout( f, result, mapKL, mapKV, solList ):
    l_SolLast = ""
    n_SolStatus = 0
    result["Number_Solutions"] = 0
    for line in f:
        line = line.rstrip()             ## To remove \n and spaces on the right
        res00 = {}               ## A temporary to see if we get a feasible solution separator
        checkKeylines( line, mapKL, res00 )
        utils.mergeDict( result, res00 )
        checkKeyvalues( line, mapKV, result )
        ## See if it's a solution status
        if "Sol_Status" in res00:
            if 1==res00[ "Sol_Status" ][0]:
                result["Number_Solutions"] += 1
##                result["Solution_Last"] = l_SolLast     ## Or save it? Need from a file then but might be great to have here
                if None!=solList:
                    solList.append( l_SolLast )
                l_SolLast = ""                         ## Clean up
        else:
            l_SolLast += line
            l_SolLast += '\n'
            
## Check if any keylines of the given 2-level dictionary equal the given line
## The 1st level gives variable name, 2nd level gives the line->value map
def checkKeylines( line, dict2, result ):
    assert dict==type(dict2)     ## What if we use OrderedDict???     TODO
    assert isinstance(result, dict)
    for key in dict2:
        if json_config.s_CommentKey!=key:
            val1 = dict2[key]
            assert dict==type(val1), "checkKeylines: key '%s': '%s'==type (%s)" % \
              ( key, type(val1), val1.__str__() )
            if line in val1:
                result[ key ] = [ val1[ line ], line ]
    
## Check if any search pattern of the given dict values is in the given line
## The key gives variable name, value gives the line->value mapping
def checkKeyvalues( line, dictVal, result ):
    assert dict==type(dictVal)     ## What if we use OrderedDict???     TODO
    assert isinstance(result, dict)
    for key in dictVal:
        if json_config.s_CommentKey!=key:
            paramArray = dictVal[ key ]
            assert 3<=len( paramArray ), \
              "Key '%s': param list should have >=3 elements, now: %s" % (key, paramArray.__str__())
            if None!=re.search( paramArray[0], line ):
                try:
                    lineSubst = re.sub( paramArray[1], ' ', line )
                except:
                    print("   WARNING: failed to substitute regex '", paramArray[1],
                          "' by ' ' in string '", line, "':  ", sys.exc_info()[0:2]
                          , sep='')
                else:
                    lSL = lineSubst.split()
                    if len( lSL )>paramArray[2]-1:
                        s_Val = lSL[ paramArray[2]-1 ]
 ##               d_Val = try_float( s_Val ) 
                        result[key] = s_Val             ### [ d_Val, s_Val ] Need here?
                    else:
                        print( "ERROR: Parsing output line ", lSL,
                              ": regex key '", paramArray[0],
                              "' found, but the split line too short for index ", paramArray[2],
                              sep='' )
