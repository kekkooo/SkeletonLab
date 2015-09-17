#include "bufferStatus.h"

Status::Status(Skel::CurveSkeleton currentSkel, QList<int> currentSelection)
{
	this->currentSkel = currentSkel;
	this->currentSelection = currentSelection;
}

Skel::CurveSkeleton Status::getSkeleton()
{
	return currentSkel;
}

QList<int> Status::getSelection()
{
	return currentSelection;
}

void Status::setSkeleton(Skel::CurveSkeleton skeleton)
{
	currentSkel = skeleton;
}

void Status::setSelection(QList<int> selection)
{
	currentSelection = selection;
}

BufferStatus::BufferStatus()
{
	// The bufferIndex must be initalized to -1 because when the first
	// state will be inserted, it will be inserted with the 0 index.
	this->bufferIndex = -1;
}

BufferStatus::BufferStatus(Status status)
{
	this->bufferIndex = -1;
	this->addState(status);
}

void BufferStatus::addState(Status status)
{
	// When a new state is inserted, all the reverted status will be deleted
	// before inserting	a new status.
	while(buffer.size() > (bufferIndex + 1) )
	{
		buffer.pop_back();
	}

	bufferIndex += 1;

	buffer.push_back(status);
}

void BufferStatus::addState(Skel::CurveSkeleton skel, QList<int> selection)
{
	while(buffer.size() > (bufferIndex + 1) )
	{
		buffer.pop_back();
	}

	bufferIndex += 1;

	Status status(skel, selection);

	buffer.push_back(status);
}

void BufferStatus::clean()
{
	// The buffer is emptied.
	buffer.clear();

	// The bufferIndex is resetted to -1 to be prepared to the first insertion.
	bufferIndex = -1;
}

int BufferStatus::getBufferIndex()
{
	return bufferIndex;
}

Skel::CurveSkeleton BufferStatus::revertSkeleton(int step)
{
	// Check if the step is in a correct range.
	assert(step < buffer.size() && step > 0);

	bufferIndex -= step;

	return buffer[bufferIndex].getSkeleton();
}

QList<int> BufferStatus::revertSelection(int step)
{
	assert(step < buffer.size() && step > 0);

	return buffer[bufferIndex].getSelection();
}

Status BufferStatus::revert(int step)
{
	assert(step < buffer.size() && step > 0);

	bufferIndex -= step;

	return buffer[bufferIndex];
}

Skel::CurveSkeleton BufferStatus::restoreSkeleton(int step)
{
	assert(step < buffer.size() && step > 0);

	bufferIndex += step;

	return buffer[bufferIndex].getSkeleton();
}

QList<int> BufferStatus::restoreSelection(int step)
{
	assert(step < buffer.size() && step > 0);

	return buffer[bufferIndex].getSelection();
}

Status BufferStatus::restore(int step)
{
	assert(step < buffer.size() && step > 0);

	bufferIndex += step;

	return buffer[bufferIndex];
}

int BufferStatus::getSize()
{
	return buffer.size();
}
