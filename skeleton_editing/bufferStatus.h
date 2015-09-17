#include <QList>
#include <skel/base.h>

// Status class is a container for a system skeleton status.
// It contains a version of the skeleton and a selection.
class Status
{
public:
	Status(Skel::CurveSkeleton currentSkel, QList<int> currentSelection);

	Skel::CurveSkeleton getSkeleton();
	QList<int> getSelection();

	void setSkeleton(Skel::CurveSkeleton skeleton);
	void setSelection(QList<int> selection);

private:

	Skel::CurveSkeleton currentSkel;
	QList<int> currentSelection;
};

// The BufferStatus class is the buffer for all the saved statuses of the system.
// It contains a list of all previous skeleton edits that can be restored and the
// edits undone that can be resumed.
class BufferStatus
{
public:

	BufferStatus();
	BufferStatus(Status status);

	// The addState method add a state to the buffer. A status can be directly inserted
	// into the buffer or a new one is created from the skeleton and the selection
	// list passed to the method.
	void addState(Status status);
	void addState(Skel::CurveSkeleton skel, QList<int> selection);

	// The clean method reset the buffer.
	void clean();

	int getBufferIndex();
	int getSize();

	// The following methods return a previous state (or its skeleton/selection).
	Status revert(int step = 1);
	Skel::CurveSkeleton revertSkeleton(int step = 1);
	QList<int> revertSelection(int step = 1);

	// The following methods return a previously reverted state (or its skeleton/selection).
	Status restore(int step = 1);
	Skel::CurveSkeleton restoreSkeleton(int step = 1);
	QList<int> restoreSelection(int step = 1);

private:

	// The bufferIndex point to the index of the current status in the buffer.
	int bufferIndex;
	QList<Status> buffer;
};
