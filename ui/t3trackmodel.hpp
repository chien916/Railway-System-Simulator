#ifndef T3TRACKMODEL_H
#define T3TRACKMODEL_H


#include <QtCore>
class T3TrackHandler {
  public:
	static bool connectedToTrackController;
	static bool connectedToTrainModel;
	static void plcIterate(QVarLengthArray<QPair<const QJsonObject*, QJsonObject*>, 5>& quintupleBlocks);
};

inline void T3TrackHandler::plcIterate(QVarLengthArray<QPair<const QJsonObject*, QJsonObject*>, 5>& quintupleBlocks) {
	Q_ASSERT(quintupleBlocks.size() == 5);
	bool currHasCrossing = quintupleBlocks.at(2).second->value("crossing").toBool();

	//get occupancy of prev2 prev1 curr next1 next2
	bool prev2occupancy = quintupleBlocks.at(0).second->value("trainOnBlock").toString().compare("") != 0;
	bool prev1occupancy = quintupleBlocks.at(1).second->value("trainOnBlock").toString().compare("") != 0;
	bool curroccupancy = quintupleBlocks.at(2).second->value("trainOnBlock").toString().compare("") != 0;
	bool next1occupancy = quintupleBlocks.at(3).second->value("trainOnBlock").toString().compare("") != 0;
	bool next2occupancy = quintupleBlocks.at(4).second->value("trainOnBlock").toString().compare("") != 0;

	QString reversedLight = prev1occupancy ? "stop" : (prev2occupancy ? "approach" : "clear");
	QString forwardLight = next1occupancy ? "stop" : (next1occupancy ? "approach" : "clear");
	QString crossing = currHasCrossing
					   ? ((prev2occupancy || prev1occupancy || curroccupancy || next1occupancy || next2occupancy)
						  ? "down" : "up")
					   : "";

	quintupleBlocks.at(2).second->insert("forwardLight", forwardLight);
	quintupleBlocks.at(2).second->insert("reversedLight", reversedLight);
	quintupleBlocks.at(2).second->insert("crossingPosition", crossing);
}
#endif // T3TRACKMODEL_H
