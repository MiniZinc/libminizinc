#include <minizinc/solvers/nl/nl_solreader.hh>

namespace MiniZinc {
  
  // *** *** *** NLSol *** *** ***

  // Parse a string into a NLSol object
  NLSol NLSol::parse_sol(istream &in){
    string            buffer;
    string            msg;
    vector<double>    vec;
    NL_Solver_Status  st;
    

    try {

      // Read the message
      while(getline(in, buffer) && !buffer.empty()){
        msg += buffer + '\n';
      }

      if (in.bad()){
        return NLSol("Error reading the solver message", NL_Solver_Status::PARSE_ERROR, {});
      }

      // Check if we have 'Options', and skip them
      if(getline(in, buffer)){
        if(buffer == "Options"){
          if(getline(in, buffer)){
            int nb_options = stoi(buffer);
            while(nb_options>0){
              getline(in, buffer);
              nb_options--;
            }
          }
        }
      }

      if (in.bad()){
        return NLSol("Error reading the solver Options", NL_Solver_Status::PARSE_ERROR, {});
      }

      // Check the dual: we ignore the first, read the second. If non zero, some lines are to be skipped
      (getline(in, buffer) && getline(in, buffer));
      if(in.bad()){
        return NLSol("Error reading the number of dual", NL_Solver_Status::PARSE_ERROR, {});
      }
      int nb_duals = stoi(buffer);

      // Check the primal: we ignore the first, read the second
      getline(in, buffer) && getline(in, buffer);
      if (in.bad()){
        return NLSol("Error reading the number of primal", NL_Solver_Status::PARSE_ERROR, {});
      }
      int nb_vars = stoi(buffer);

      // Skip the duals
      while(nb_duals>0 && getline(in, buffer)){
        nb_duals--;
      }

      if (in.bad()){
        return NLSol("Error reading the dual values", NL_Solver_Status::PARSE_ERROR, {});
      }

      // Read the vars
      while(nb_vars>0 && getline(in, buffer)){
        double d = stod(buffer);
        vec.push_back(d);
        nb_vars--;
      }

      if (in.bad()){
        return NLSol("Error reading the primal values", NL_Solver_Status::PARSE_ERROR, {});
      }

      // Reading status code
      // objno 0 EXIT
      // ........
      // 8 char
      getline(in, buffer);
      string sub = buffer.substr(8, buffer.length()-8); 

      switch(stoi(sub)){
        //Not this case, this one is our own: case -2: st = NL_Solver_Status::PARSE_ERROR;
        case -1: st = NL_Solver_Status::UNKNOWN; break;
        case 0: st = NL_Solver_Status::SOLVED; break;
        case 100: st = NL_Solver_Status::UNCERTAIN; break;
        case 200: st = NL_Solver_Status::INFEASIBLE; break;
        case 300: st = NL_Solver_Status::UNBOUNDED; break;
        case 400: st = NL_Solver_Status::LIMIT; break;
        case 500: st = NL_Solver_Status::FAILURE; break;
        case 600: st = NL_Solver_Status::INTERRUPTED; break;
        default: st = NL_Solver_Status::UNKNOWN; break;
      }


    } catch(...){
      return NLSol("Parsing error (probably a bad number)", NL_Solver_Status::PARSE_ERROR, vec);
    }

    return NLSol(msg, st, vec);
  }


  // *** *** *** NLSolns2Out *** *** ***

  /** Our "feedrawdatachunk" directly gets the solver's output, which is not the result.
   *  The result is written in the .sol file.
   *  Get the solver output and add a comment % in front of the lines
   *  We may be in a middle of a line!
   */
  bool NLSolns2Out::feedRawDataChunk(const char* data) {
    if(data != NULL){
      std::stringstream ss(data);
      string to;

      while(getline(ss, to)){
        if(ss.eof()){
          if(in_line){ // Must complete a line, and the line is not over yet
            getLog() << to << endl;
          } else {  // Start an incomple line
            getLog() << "% " << to;
            in_line = true;
          }
        } else {
          if(in_line){ // Must complete a line, and the line is over.
            getLog() << to << endl;
            in_line = false;
          } else { // Full line
            getLog() << "% " << to << endl;
          }
        }
      }

    }
    return true;
  }


  void  NLSolns2Out::parse_sol(const string& filename){

    ifstream  f(filename);
    NLSol sol = NLSol::parse_sol(f);

      switch(sol.status){

        case NL_Solver_Status::PARSE_ERROR:{
          DEBUG_MSG("NL msg: PARSE ERROR" << endl);
          out->feedRawDataChunk(out->_opt.error_msg_00);
          break;
        }

        case NL_Solver_Status::UNKNOWN:{
          DEBUG_MSG("NL msg: UNKNOWN" << endl);
          out->feedRawDataChunk(out->_opt.unknown_msg_00);
          break;
        }

        case NL_Solver_Status::SOLVED:{
           DEBUG_MSG("NL msg: SOLVED" << endl);

          stringstream sb;
          sb << std::showpoint; // Always shows the decimal point, so we have e.g. '256.0' when 256 is the answer for a fp value.
          sb.precision(numeric_limits<double>::digits10 + 2);

          for(int i=0; i<nl_file.variables.size(); ++i){
            string n = nl_file.vnames[i];
            NLVar v = nl_file.variables[n];   
            if(v.to_report){
                sb << v.name << " = ";
                if(v.is_integer){
                  long value = sol.values[i];
                  sb << value;
                } else {
                  double value = sol.values[i];
                  sb << value;
                }
                sb << ";\n";
            }
          }

          // Output the arrays
          for(int i=0; i<nl_file.output_arrays.size(); ++i){
            NLArray &a = nl_file.output_arrays.at(i);
            sb << a.name << " = array" << a.dimensions.size() << "d( ";
            for(const string& s: a.dimensions){
              sb << s << ", ";
            }
            sb << "[";
            for(int j=0; j<a.items.size(); ++j){
              const NLArray::Item &item = a.items.at(j);

              // Case of the literals
              if(item.variable.empty()){
                if(a.is_integer){
                  long value = item.value;
                  sb << value;
                } else {
                  double value = item.value;
                  sb << value;
                }
              } else {
                int index = nl_file.variable_indexes.at(item.variable);
                if(a.is_integer){
                  long value = sol.values[index];
                  sb << value;
                } else {
                  double value = sol.values[index];
                  sb << value;
                }
              }

              if(j<a.items.size()-1){
                sb << ", ";
              }

            }

            sb << "]);\n";
          }

          string s = sb.str();
          out->feedRawDataChunk(s.c_str());
          out->feedRawDataChunk(out->_opt.solution_separator_00);
          if(nl_file.objective.is_optimisation()){
            out->feedRawDataChunk("\n");
            out->feedRawDataChunk(out->_opt.search_complete_msg_00);
          }

          break;
        }

        case NL_Solver_Status::UNCERTAIN:{
          DEBUG_MSG("NL msg: UNCERTAIN" << endl);
          out->feedRawDataChunk(out->_opt.unknown_msg_00);
          break;
        }

        case NL_Solver_Status::INFEASIBLE:{
          DEBUG_MSG("NL msg: INFEASIBLE" << endl);
          out->feedRawDataChunk(out->_opt.unsatisfiable_msg_00);
          break;
        }

        case NL_Solver_Status::UNBOUNDED:{
          DEBUG_MSG("NL msg: UNBOUNDED" << endl);
          out->feedRawDataChunk(out->_opt.unbounded_msg_00);
          break;
        }

        case NL_Solver_Status::LIMIT:{
         DEBUG_MSG("NL msg: LIMIT" << endl);
          out->feedRawDataChunk(out->_opt.unknown_msg_00);
          break;
        }

        case NL_Solver_Status::INTERRUPTED:{
         DEBUG_MSG("NL msg: INTERRUPTED" << endl);
          out->feedRawDataChunk(out->_opt.unknown_msg_00);
          break;
        }

        default: should_not_happen("parse_sol: switch on status with unknown code: " << sol.status);
      } 

      // "Finish" the feed
      out->feedRawDataChunk("\n");

  }

  ostream& NLSolns2Out::getLog(void) {
    return out->getLog();
  }

}