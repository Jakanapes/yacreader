#include "volume_comics_model.h"
#include "qnaturalsorting.h"


#include <QtScript>

bool lessThan(const QList<QString> & left, const QList<QString> & right)
{
	if ((left.count() > 0) && (right.count() > 0))
		return naturalSortLessThanCI(left.at(0),right.at(0));
	else
		return true;
}

VolumeComicsModel::VolumeComicsModel(QObject * parent) :
	JSONModel(parent),numExtraRows(0)
{
}

void VolumeComicsModel::load(const QString & json)
{
	QScriptEngine engine;
	QScriptValue sc;
	sc = engine.evaluate("(" + json + ")");

	if (!sc.property("error").isValid() && sc.property("error").toString() != "OK")
	{
		qDebug("Error detected");
	}
	else
	{
		int numResults = sc.property("number_of_total_results").toString().toInt(); //fix to weird behaviour using hasNext
		QScriptValueIterator it(sc.property("results"));
		//bool test;
		QScriptValue resultsValue;
		while (it.hasNext()) {
			it.next();
			resultsValue = it.value();
			QString issueNumber = resultsValue.property("issue_number").toString();
			QString name = resultsValue.property("name").toString();
			QString id = resultsValue.property("id").toString();
			QStringList l;
			l << issueNumber << name << id;
			//test = name.isEmpty() && year.isEmpty() && numIssues.isEmpty() && url.isEmpty();
			if(numResults > 0)
				_data.push_back(l);
			numResults--;
		}

		qSort(_data.begin(),_data.end(),lessThan);
	}
}

QModelIndex VolumeComicsModel::parent(const QModelIndex &index) const
{
	Q_UNUSED(index)
	return QModelIndex(); //no parent
}

int VolumeComicsModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return _data.count() + numExtraRows;
}

int VolumeComicsModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	if(_data.isEmpty())
		return 0;
	else
		return 2;
}

QVariant VolumeComicsModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	int row = index.row();
	int column = index.column();

	if (role == Qt::DecorationRole)
	{
			return QVariant();
	}
	if (role == Qt::TextAlignmentRole)
	{
		switch(column)//TODO obtener esto de la query
		{
		case ISSUE:
			return QVariant(Qt::AlignRight | Qt::AlignVCenter);
		case TITLE:
			return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
		}
	}

	if(role != Qt::DisplayRole)
		return QVariant();

	if(row<_data.count())
		return _data[row][column];
	else
		return QVariant();
}

Qt::ItemFlags VolumeComicsModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant VolumeComicsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch(section)//TODO obtener esto de la query
		{
		case ISSUE:
			return QVariant(QString("issue"));
		case TITLE:
			return QVariant(QString(tr("title")));
		}
	}

	if (orientation == Qt::Horizontal && role == Qt::TextAlignmentRole)
	{
		switch(section)//TODO obtener esto de la query
		{
		case ISSUE:
			return QVariant(Qt::AlignRight | Qt::AlignVCenter);
		case TITLE:
			return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
		}
	}

	return QVariant();
}

QModelIndex VolumeComicsModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	return createIndex(row, column);
}

QString VolumeComicsModel::getComicId(const QModelIndex &index) const
{
	return _data[index.row()][ID];
}

void VolumeComicsModel::addExtraRows(int numRows)
{
	numExtraRows = numRows;
}

