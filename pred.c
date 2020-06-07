
void doPredCalc(int player) {
	trajectory_t tr_tmp;
	int delta_time;
	
	
	tr_tmp.trType = history[player][0].trType;
        tr_tmp.trTime = eth.cg_time;                                                                                                                       
        VectorCopy(history[player][0].trPos, tr_tmp.trBase); 
        VectorCopy(history[player][0].trDelta, tr_tmp.trDelta);
	VectorNormalize(tr_tmp.trDelta);

	int tmp_dt = history[player][0].t - history[player][1].t;
	if (tmp_dt > 0 && tmp_dt < 500)
	    delta_time = (eth.cg_snap->serverTime - eth.cg_time) / (history[player][0].t - history[player][1].t);
	else if(eth.cg_snap->ping > 0)
	    delta_time = (eth.cg_snap->serverTime - eth.cg_time) / eth.cg_snap->ping;
	else 	    
	    delta_time = (eth.cg_snap->serverTime - eth.cg_time)/50;
	eth.BG_EvaluateTrajectory(&tr_tmp,(eth.cg_snap->serverTime + delta_time) + (int)seth.value[VAR_PRED_C],history[player][0].prPos,qfalse,-1);

	if(VectorDistance(history[player][0].prPos,history[player][0].trPos) < 100)
	    VectorSubtract(history[player][0].prPos,history[player][0].trPos,history[player][0].prPos);
	else 
	    VectorCopy(history[player][0].trDelta,history[player][0].prPos);
	    
	VectorNormalize(history[player][0].prPos);
	

	vec3_t distance;
	VectorAdd(history[player][1].trPos,history[player][1].prPos,distance);
	
	eth.predMissed = VectorDistance(distance,history[player][0].trPos);	
}
