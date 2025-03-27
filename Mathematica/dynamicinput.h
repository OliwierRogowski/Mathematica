#ifndef DYNAMICINPUT_H
#define DYNAMICINPUT_H

#include <QLineEdit>
#include <QList>

class DynamicInput {
public:
    DynamicInput() : nextId(1) {}

    // Adding new input to list
    void addInput(QLineEdit *input) {
        inputs.append({nextId++, input});
    }

    QLineEdit* getInputFieldById(int id) {
        for (const auto &entry : inputs) {
            if (entry.id == id)
                return entry.inputField;
        }
        return nullptr;
    }

    QString getInputTextById(int id) {
        QLineEdit *input = getInputFieldById(id);
        return input ? input->text().trimmed() : "";
    }

    // Return a list of just QLineEdit* objects
    QVector<QLineEdit*> getInputFields() const {
        QVector<QLineEdit*> inputFields;
        for (const auto &entry : inputs) {
            inputFields.append(entry.inputField);
        }
        return inputFields;
    }

    struct InputEntry {
        int id;
        QLineEdit *inputField;
    };

    QList<InputEntry> inputs;
    int nextId;  // This will be the unique identifier for each input field

};

#endif // DYNAMICINPUT_H
