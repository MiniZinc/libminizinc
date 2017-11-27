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
        self.hdrSummary = [       ## These are column headers for overall per-method summary
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
        self.hdrRanking = [       ## These are column headers for ranking analysis
            ## ( "nmMeth", "logfile/test/method name" ),
            ( "OOpt",   "Number of instances where ONLY this method is OPTIMAL" ),
            ( "OSaC",   "Number of instances where ONLY this method is SAT-COMPLETE" ),
            ( "OFeas",  "Number of instances where ONLY this method is FEASIBLE and none is optimal" ),
            ( "OSat",   "Number of instances where ONLY this method is SAT and none is optimal" ),
            ( "OInfeas","Number of instances where ONLY this method is INFeasible" ),
            ( "BPri",   "Number of instances where this method has a better PRIMAL BOUND" ),
            ( "BDua",   "Number of instances where this method has a better DUAL BOUND" )
            ##  TODO: ranks here ( "n_UNKNOWN",    "Nunkn" )
          ]
        
    ## Add a method's log
    def addLog( self, sName ):
        self.lResLogs.append( ( {}, [ sName, '' ] ) )      ## a tuple of dict and list of names ([filename, nick])
        return self.getLastLog()
        
    def getLastLog( self ):
        assert 0<len( self.lResLogs )
        return self.lResLogs[-1]

    def getMethodName( self, lNames ):    ## produce a sigle string from a list to create a method name
        return " ".join( lNames )
      
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
        self.nContrStatus, self.nContrOptVal, self.nContrBounds = 0, 0, 0
        self.matrRanking = utils.MatrixByStr(
                [ ( self.getMethodName( lNames ), "No long name" ) for mLog, lNames in self.lResLogs ],
                self.hdrRanking )
        self.matrRankingMsg = utils.MatrixByStr(
                [ ( self.getMethodName( lNames ), "No long name" ) for mLog, lNames in self.lResLogs ],
                self.hdrRanking, [] )

        self.nNoOptAndAtLeast2Feas = 0

        ############################## Output strings. TODO into a map
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

    ## Summarize comparisons / ranking
    def summarizeCmp( self ):
        print(
            self.matrRankingMsg.stringifyLists( "       METHODS' STAND-OUTS",
                    "        METHOD",
                    "        PARAMETER" ) + \
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
            self.ioErrors.getvalue() + "\n" + \
            "\n\n------------------ RANKING ------------------\n\n" + \
            self.matrRanking.stringify2D()
        )
    
    ## Summarize
    def summarize( self ):
        return \
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
        self.lOpt, self.lSatAll, self.lFeas, self.lSat, self.lInfeas = [], [], [], [], []
        self.mOptVal, self.lOptVal, self.lPrimBnd, self.lDualBnd = {}, [], [], []
        self.nReported = 0               ## How many methods reported for this instances

    def tryFindProblemSense( self, sInst ):
        self.sSenses = {}
        self.nOptSenseGiven = -2;
        for mLog, lNames in self.lResLogs:
            if sInst in mLog:
                mSlv = mLog[ sInst ][ "__SOLVE__" ]               ## __SOLVE__ always there???
                if "Problem_Sense" in mSlv:
                    self.sSenses[ mSlv["Problem_Sense"][0] ] = lNames # mSlv["Problem_Sense"][1]
        if 1<len( self.sSenses ):
            self.lInstContradOptSense.append( sInst )
            print( "WARNING: DIFFERENT OBJ SENSES REPORTED for the instance ", sInst,
                   ":  ", self.sSenses, sep='', file=ioContrSense )
        elif 1==len( self.sSenses ):
            self.nOptSenseGiven = list(self.sSenses.keys())[0]
                
    def compileInstanceResults( self, sInst ):
        for mLog, lN in self.lResLogs:                ## Select method and its name list
            lNames = " ".join(lN)
            if sInst in mLog:
                self.nReported += 1
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
                    ### ... here assumed it's an opt problem by default... why... need to check bounds first??
                ## Handle optimality / SAT completed
                if 2==n_SolStatus:
                    if not self.bOptProblem:
                        self.lSatAll.append( lNames )
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


    ###
    ### Now compare between differen methods: CONTRADICTIONS
    ###
    def checkContradictions( self, sInst ):
        self.fContr = False
        if len(self.lOpt)+len(self.lSat) > 0 and len(self.lInfeas) > 0:
            self.nContrStatus += 1
            self.fContr = True
            print(  "CONTRADICTION of STATUS: instance " + str(sInst) + ": " + \
                   "\n  OPTIMAL:  " + str(self.lOpt) + \
                   "\n  FEAS:  " + str(self.lSat) + \
                   "\n  INFEAS:  " + str(self.lInfeas), file= self.ioContrStatus )
        if len(self.mOptVal) > 1:
            self.nContrOptVal += 1
            self.fContr = True
            print( "CONTRADICTION of OPTIMAL VALUES: " + str(sInst) + \
              ": " + str(self.mOptVal), file=self.ioContrOptVal )
        self.nOptSense=0;      ## Take as SAT by default
        if len(self.lPrimBnd)>0 and len(self.lDualBnd)>0 and len(self.lOpt)<self.nReported:
            lKeysP, lValP = zip(*self.lPrimBnd)
            lKeysD, lValD = zip(*self.lDualBnd)
            nPMin, nPMax, nDMin, nDMax = \
                min(lKeysP), max(lKeysP), \
                min(lKeysD), max(lKeysD)
            if nPMax <= nDMin + 1e-6 and nPMin < nDMax - 1e-6:
                self.nOptSense=1                             ## maximize
            elif nPMin >= nDMax - 1e-6 and nPMax > nDMin + 1e-6:
                self.nOptSense=-1                            ## minimize
            elif nPMax > nDMin + 1e-6 and nPMin < nDMax - 1e-6 or \
               nPMin < nDMax - 1e-6 and nPMax > nDMin + 1e-6:
                self.nContradBounds += 1
                self.fContr = True
                print( "CONTRADICTION of BOUNDS: instance " + str(sInst) + \
                  ":\n  PRIMALS: " + str(self.lPrimBnd) + ",\n  DUALS: " + str(self.lDualBnd),
                  file = self.ioContrBounds )
            else:
                self.nOptSense=0          ## SAT
            if 1==len(self.sSenses) and self.nOptSense!=0:
                if self.nOptSense!=self.nOptSenseGiven:     ## access the 'given' opt sense
                    print( "CONTRADICITON of IMPLIED OBJ SENSE:  Instance "+ str(sInst) + \
                      ": primal bounds " + str(self.lPrimBnd) + \
                      " and dual bounds "+ str(self.lDualBnd) + \
                      " together imply opt sense " + str(self.nOptSense) + \
                      ",  while result logs say "+  str(self.nOptSenseGiven), file=self.ioContrBounds )
                ## else accepting nOptSense as it is

      
      
    ###
    ### Now compare between differen methods: DIFFERENCES AND RANKING
    ###
    def rankPerformance( self, sInst ):
        ### Accepting the opt sense from result tables, if given
        if self.nOptSenseGiven!=-2:
            self.nOptSense = self.nOptSenseGiven
        ### Compare methods on this instance:
        if not self.fContr and self.nReported == len(self.lResLogs):
            if len(self.lOpt) == 1:
                self.matrRanking[self.lOpt[0], "OOpt"] += 1
                self.matrRankingMsg[self.lOpt[0], "OOpt"].append( \
                  str(sInst) + ":   the ONLY OPTIMAL")
            elif len(self.lSatAll) == 1:
                self.matrRanking[self.lSatAll[0], "OSaC"] += 1
                self.matrRankingMsg[self.lSatAll[0], "OSaC"].append( \
                  str(sInst) + ":   the ONLY SAT-COMPLETE")
            elif len(self.lOpt) == 0 and len(self.lFeas) == 1:
                self.matrRanking[self.lFeas[0], "OFeas"] += 1
                self.matrRankingMsg[self.lFeas[0], "OFeas"].append( \
                  str(sInst) + ":   the ONLY FEASIBLE")
            elif len(self.lSatAll) == 0 and len(self.lSat) == 1:
                self.matrRanking[self.lSat[0], "OSat"] += 1
                self.matrRankingMsg[self.lSat[0], "OSat"].append( \
                  str(sInst) + ":   the ONLY SAT")
            elif len(self.lOpt) == 0 and len(self.lFeas) > 1:
                self.nNoOptAndAtLeast2Feas += 1
            elif len(self.lInfeas) == 1:
                self.matrRanking[self.lInfeas[0], "OInfeas"] += 1
                self.matrRankingMsg[self.lInfeas[0], "OInfeas"].append( \
                  str(sInst) + ":   the ONLY INFeasible")
        if not self.fContr \
           and 0==len(self.lInfeas) and 1<len(self.lFeas) and 0!=self.nOptSense:
            self.lPrimBnd.sort()
            if self.nOptSense>0:
                self.lPrimBnd.reverse()
            dBnd, dNM = zip(*self.lPrimBnd)
            dBetter = (dBnd[0]-dBnd[1]) * self.nOptSense
            if 1e-2 < dBetter:                   ## Param?  TODO
                self.matrRanking[dNM[0], "BPri"] += 1
                self.matrRankingMsg[dNM[0], "BPri"].append( str(sInst) \
                + ":      the best OBJ VALUE   by " + str(dBetter) \
                + "\n      PRIMAL BOUNDS AVAILABLE: " + str(self.lPrimBnd))
        if not self.fContr \
           and 0==len(self.lInfeas) and 1<len(self.lDualBnd) and 0!=self.nOptSense:
            self.lDualBnd.sort()
            if self.nOptSense<0:
                self.lDualBnd.reverse()
            dBnd, dNM = zip(*self.lDualBnd)
            dBetter = (dBnd[1]-dBnd[0]) * self.nOptSense
            if 1e-2 < dBetter:                   ## Param/adaptive?  TODO
                self.matrRanking[dNM[0], "BDua"] += 1
                self.matrRankingMsg[dNM[0], "BDua"].append( str(sInst) \
                + ":      the best DUAL BOUND   by " + str(dBetter) \
                + "\n      DUAL BOUNDS AVAILABLE: " + str(self.lDualBnd))
      
      
