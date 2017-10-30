##  This class compares and summarizes instance solution results
##  for a given set of optimization/satisfiability instances.
##  Looks for contradictions. Heuristic performance indicators. 
##  (c) Gleb Belov@monash.edu  2017

from collections import OrderedDict
# import prettytable
import utils, functools, io

#####################################################################################
################### class CompareLogs ##################
#####################################################################################
## It receives a list of dictionaries, keyed by instances files,
## containing values of solutions status etc., and performs the checks + summary.
class CompareLogs:
    def __init__( self ):
        self.lResLogs = []                                 ## empty list of logs/methods to compare
        self.hdrSummary = [
            ( "s_MethodName", "logfile/test name" ),
            ( "n_Reported",   "Nout" ),
            ( "n_CheckFailed","Nbad" ),
            ( "n_Errors",     "Nerr" ),
            ( "n_OPT",        "Nopt" ),
            ( "n_FEAS",       "Nfea" ),
            ( "n_SATALL",     "NsatA" ),
            ( "n_SAT",        "Nsat" ),
            ( "n_INFEAS",     "Ninfeas" ),
            ( "n_NOFZN",      "NoFZN" ),
            ( "n_UNKNOWN",    "Nunkn" )
          ]
        
    ## Add a method's log
    def addLog( self, sName ):
        self.lResLogs.append( ( {}, [ sName, '' ] ) )      ## a tuple of dict and list of names ([filename, nick])
        return self.getLastLog()
        
    def getLastLog( self ):
        assert 0<len( self.lResLogs )
        return self.lResLogs[-1]
      
    ## Return the union of all instances in all logs
    def getInstanceUnion( self ):
        return list( functools.reduce(set.union, (set(d[0].keys()) for d in self.lResLogs)) )

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
        self.nInstWithOptSense = 0
        ## Using warning strings...     self.lInstContradOptSense = []
        self.lCmpVecs = []                  # List of summary vectors for each method
        self.mCmpVecVals = {}               # Maps to the "quantity" parts of those
        self.mCmpVecQual = {}               # Maps to the "quality" parts
        for mLog, lN in self.lResLogs:
            lNames = " ".join(lN)
            av = OrderedDict({ "s_MethodName": lNames })
            aq = OrderedDict({ "s_MethodName": lNames })
            self.lCmpVecs.append( ( lNames, av, aq ) )
            self.mCmpVecVals[ lNames ] = av
            self.mCmpVecQual[ lNames ] = aq
            
        self.mInfeas, self.mNoFZN, self.mFail, self.mError = {}, {}, {}, {}
        self.nCntrStatus, self.nContrOptVal, self.nContrBounds = 0, 0, 0

        ############################## Output strings
        self.ioContrSense = io.StringIO() 
        self.ioContrStatus = io.StringIO()
        self.ioContrObjValMZN = io.StringIO()
        self.ioBadObjValueStatusOpt = io.StringIO()
        self.ioBadObjValueStatusFeas = io.StringIO()
        self.ioContrOptVal = io.StringIO()
        self.ioContrBounds = io.StringIO()
        self.ioBadChecks = io.StringIO()
        self.ioErrors = io.StringIO()
      
    ## Compare methods on the given instance
    def compareInstance( self, sInst ):
        self.initInstanceComparison( sInst )
        self.tryFindProblemSense( sInst )
        self.compileInstanceResults( sInst )
        self.checkContradictions( sInst )
        self.rankPerformance( sInst )
      
    ## Summarize up to current instance
    def summarizeCurrent( self, lLogNames ):
        lNames = " ".join(lLogNames)
        mCmpVals = self.mCmpVecVals[lNames]
        for hdr in self.hdrSummary:
            print( hdr[1], ":",
                  mCmpVals[hdr[0]] if hdr[0] in mCmpVals else 0,
                  sep='', end=' ' )
    
    ## Summarize
    def summarize( self ):
        return \
            "\n------------------ SOLUTION CHECKS FAILURES ------------------\n\n" + \
            self.ioBadChecks.getvalue() + \
            "\n\n------------------ OBJECTIVE SENSE CONTRADICTIONS ------------------\n\n" + \
            self.ioContrSense.getvalue() + \
            "\n\n------------------ OBJECTIVE VALUE MZN / SOLVER CONTRADICTIONS ------------------\n\n" + \
            self.ioContrObjValMZN.getvalue() + \
            "\n\n------------------ OBJECTIVE VALUE BAD, STATUS OPTIMAL ------------------\n\n" + \
            self.ioBadObjValueStatusOpt.getvalue() + \
            "\n\n------------------ OBJECTIVE VALUE BAD, STATUS FEASIBLE ------------------\n\n" + \
            self.ioBadObjValueStatusFeas.getvalue() + \
            "\n\n------------------ STATUS CONTRADICTIONS ------------------\n\n" + \
            self.ioContrStatus.getvalue() + \
            "\n\n------------------ OBJECTIVE VALUE CONTRADICTIONS ------------------\n\n" + \
            self.ioContrOptVal.getvalue() + \
            "\n\n------------------ DUAL BOUND CONTRADICTIONS ------------------\n\n" + \
            self.ioContrBounds.getvalue() + \
            "\n\n------------------ ERRORS REPORTED BY SOLVERS ------------------\n\n" + \
            self.ioErrors.getvalue() + \
            "\n\n------------------ SUMMARY TABLE ------------------\n\n" + \
            utils.MyTab().tabulate(
              [ [ lcv[1][hdr[0]] if hdr[0] in lcv[1] else 0
                  for hdr in self.hdrSummary ]
                for lcv in self.lCmpVecs ],
                [ pr[1] for pr in self.hdrSummary ]
            )
      
###############################################################################################
####################### LEVEL 2 #########################
###############################################################################################
    def initInstanceComparison( self, sInst ):
        self.lOpt, self.lFeas, self.lSat, self.lInfeas = [], [], [], []
        self.mOptVal, self.lOptVal, self.lPrimBnd, self.lDualBnd = {}, [], [], []

    def tryFindProblemSense( self, sInst ):
        self.sSenses = {}
        for mLog, lNames in self.lResLogs:
            if sInst in mLog:
                mSlv = mLog[ sInst ][ "__SOLVE__" ]               ## __SOLVE__ always there???
                if "Problem_Sense" in mSlv:
                    self.sSenses[ mSlv["Problem_Sense"][0] ] = lNames # mSlv["Problem_Sense"][1]
        if 1<len( self.sSenses ):
            self.lInstContradOptSense.append( sInst )
            print( "WARNING: DIFFERENT OBJ SENSES REPORTED for the instance ", sInst,
                   ":  ", self.sSenses, sep='', file=ioContrSense )
                
    def compileInstanceResults( self, sInst ):
        for mLog, lN in self.lResLogs:                ## Select method and its name list
            lNames = " ".join(lN)
            if sInst in mLog:
                aResultThisInst = OrderedDict({ "n_Reported": 1 })
                aResultThisInst[ "n_CheckFailed" ] = 0
                mRes = mLog[ sInst ]                      ## The method's entry for this instance
                if "SOLUTION_CHECKS_FAILED" in mRes and \
                  0<mRes["SOLUTION_CHECKS_FAILED"]:
                    aResultThisInst[ "n_CheckFailed" ] = 1
                    utils.addMapValues( self.mCmpVecVals[lNames], aResultThisInst )
                    print( "WARNING: SOLUTION CHECK(S) FAILED for the instance ", sInst,
                           ",  method '", lNames, "'.", sep='', file = self.ioBadChecks )
                    continue                                        ## TODO. Param?
                aResultThisInst[ "n_Errors" ] = 0
                mSlv = mRes[ "__SOLVE__" ]
                dObj_MZN = utils.try_float( mSlv.get( "ObjVal_MZN" ) )
                dObj_SLV = utils.try_float( mSlv.get( "ObjVal_Solver" ) )
                dBnd_SLV = utils.try_float( mSlv.get( "DualBnd_Solver" ) )
                dTime_Flt = utils.try_float( mSlv.get( "Time_Flt" ) )
                ## Compare obj vals
                dObj, bObj_MZN = (dObj_MZN, True) if \
                      None!=dObj_MZN and abs( dObj_MZN ) < 1e45 else (mSlv.get("ObjVal_MZN"), False)
                ## Assuming solver value is better if different. WHY? Well it' happened both ways
                dObj, bObj_SLV = (dObj_SLV, True) if \
                      None!=dObj_SLV and abs( dObj_SLV ) < 1e45 else (dObj, False)
                if bObj_MZN and bObj_SLV:
                    if abs( dObj_MZN-dObj_SLV ) > 1e-6 * max( abs(dObj_MZN), abs(dObj_SLV) ):
                        aResultThisInst[ "n_Errors" ] += 1
                        print ( "  WARNING: DIFFERENT MZN / SOLVER OBJ VALUES for the instance ", sInst,
                           ", method '", lNames, "' : ",
                           dObj_MZN, " / ", dObj_SLV, sep-'', file=self.ioContrObjValMZN)
                ## Retrieve solution status
                if "Sol_Status" in mSlv:
                    n_SolStatus = mSlv[ "Sol_Status" ][0]
                else:
                    n_SolStatus = 0
                ## Retrieve dual bound
                dBnd = None
                if None!=dBnd_SLV and abs( dBnd_SLV ) < 1e45:
                    dBnd = dBnd_SLV
                    self.lDualBnd.append( ( dBnd_SLV, lNames ) )      ## Even infeas instances can have dual bound?
                ## Trying to deduce opt sense if not given:
                if 1==len(self.sSenses):
                    nSense = next(iter(self.sSenses.keys()))
                else:
                    nSense = -2  ## ??
                self.bOptProblem = True if 0!=nSense else False     ## or (None!=dBnd or None!=dObj)
                ## Handle optimality / SAT completed
                if 2==n_SolStatus:
                    if not self.bOptProblem:
                        self.lSat.append( lNames )
                        aResultThisInst[ "n_SATALL" ] = 1
                    else:                                        ## Assume it's an optimization problem????? TODO
                        self.lOpt.append( lNames )                   ## Append the optimal method list
                        aResultThisInst[ "n_OPT" ] = 1
                        if None==dObj or abs( dObj ) >= 1e45:
                            aResultThisInst[ "n_Errors" ] += 1
                            print ( "  WARNING: OPTIMAL STATUS BUT BAD OBJ VALUE, instance ", sInst,
                              ", method '", lNames, "': '",
                              ( "" if None==dObj else str(dObj) ), "', result record: ",   # mRes,
                              ",, dObj_MZN: ", dObj_MZN, sep='', file=self.ioBadObjValueStatusOpt )
                        else:
                            self.mOptVal[ dObj ] = lNames            ## Could have used OrderedDict
                            self.lOptVal.append( (dObj, lNames) )    ## To have both a map and the order
                            self.lPrimBnd.append( (dObj, lNames) )
                ## Handle feasibility / SAT
                elif 1==n_SolStatus:
                    if not self.bOptProblem:
                        self.lSat.append( lNames )
                        aResultThisInst[ "n_SAT" ] = 1
                    else:                                        ## Assume it's an optimization problem????? TODO
                        self.lFeas.append( lNames )                   ## Append the optimal method list
                        aResultThisInst[ "n_FEAS" ] = 1
                        if None==dObj or abs( dObj ) >= 1e45:
                            aResultThisInst[ "n_Errors" ] += 1
                            print ( "  WARNING: feasible status but bad obj value, instance ", sInst,
                                    ", method '", lNames, "' :'",
                              ( "" if None==dObj else str(dObj) ), "', result record: ",  #  mRes,
                              sep='', file=self.ioBadObjValueStatusFeas )
                        else:
                            self.lPrimBnd.append( (dObj, lNames) )
                ## Handle infeasibility
                elif -1>=n_SolStatus and -3<=n_SolStatus:
                    self.lInfeas.append( lNames )
                    aResultThisInst[ "n_INFEAS" ] = 1
                    self.mInfeas. setdefault( sInst, [] )
                    self.mInfeas[ sInst ].append( lNames )
                ## Handle ERROR?
                elif -4==n_SolStatus:
                    aResultThisInst[ "n_Errors" ] = 1
                    self.mError. setdefault( sInst, [] ).append( lNames )
                    print( "ERROR REPORTED for the instance ", sInst, ", method '", lNames,
                            "',  result record: ",   ## mRes,
                            sep='', file=self.ioErrors )
                else:
                    aResultThisInst[ "n_UNKNOWN" ] = 1
                ## Handle NOFZN
                if None==dTime_Flt:
                    aResultThisInst[ "n_NOFZN" ] = 1
                    self.mNoFZN. setdefault( sInst, [] ).append( lNames )
                ## Handle FAIL???
                # LAST:
                utils.addMapValues( self.mCmpVecVals[lNames], aResultThisInst )

      
    def checkContradictions( self, sInst ):
        pass
      
    def rankPerformance( self, sInst ):
        pass
      
      
