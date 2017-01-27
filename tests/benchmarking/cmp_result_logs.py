##  This class compares and summarizes instance solution results
##  for a given set of optimization/satisfiability instances.
##  Looks for contradictions. Heuristic performance indicators. 
##  (c) Gleb Belov@monash.edu  2017

#####################################################################################
################### class CompareLogs ##################
#####################################################################################
## It receives a list of dictionaries, keyed by instances files,
## containing values of solutions status etc., and performs the checks + summary.
class CompareLogs:
    def __init__( self ):
        self.lResLogs = []                                 ## empty list of logs/methods to compare
        
    ## Add a method's log
    def addLog( self, sName ):
        self.lResLogs.append( ( {}, [ sName, '' ] ) )      ## a tuple of dict and list of names ([filename, nick])
        return self.getLastLog()
        
    def getLastLog( self ):
        assert 0<len( self.lResLogs )
        return self.lResLogs[-1]
      
    ## Return the union of all instances in all logs
    def getInstanceUnion( self ):
        assert False
        return []                      ## TODO

    ## Return the intersection of all instances in all logs
    def getInstanceIntersection( self ):
        assert False
        return []                      ## TODO

    ## This compares all instances specified in the list
    ## If comparing by-instance from outside, follow this pattern
    def compareAllInstances( self, lInstances ):
        self.initListComparison()
        for sInst in lInstances:
            self.compareInstance( sInst )
        self.summarize()
        
    ## Init stats etc.
    def initListComparison( self ):
        pass
      
    ## Compare methods on the given instance
    def compareInstance( self, sInst ):
        self.tryFindProblemSense( sInst )
        self.compileInstanceResults( sInst )
        self.checkContradictions( sInst )
        self.rankPerformance( sInst )
      
    ## Summarize
    def summarize( self ):
        pass
      
###############################################################################################
####################### LEVEL 2 #########################
###############################################################################################
    def tryFindProblemSense( self, sInst ):
        for mLog, lNames in self.lResLogs:
            if sInst in mLog:
                mRes = mLog[ sInst ]
                
    def compileInstanceResults( self, sInst ):
        for mLog, lNames in self.lResLogs:                ## Select method
            if sInst in mLog:
                mRes = mLog[ sInst ]                      ## The method's entry for this instance
                if "SOLUTION_CHECK_RESULT" in mRes and \
                  "CONTRADICTION"==self.result["SOLUTION_CHECK_RESULT"]:
                    assert False    ## NEED STATS ON THIS, for the method and overall
                    continue                                        ## TODO. Param?

      
    def checkContradictions( self, sInst ):
        pass
      
    def rankPerformance( self, sInst ):
        pass
      
      