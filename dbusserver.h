#ifndef DBUSSERVER_H
#define DBUSSERVER_H

#include <QObject>
#include <QDBusConnection>
#include <QTextEdit>

class TextEdit;

class DbusServer : public QObject
{
    Q_OBJECT
private:
    TextEdit* te_ = nullptr;

    /*!
     * \brief dbus_interface_name_ - имя и версия интерфейса D-Bus
     */
    QString dbus_server_name_ = "ru.textedit.dbus1";

    /*!
     * \brief registrate_new_server - этот экземпляр приложения зарегистрировал сервер иотвечает за него
     */
    bool registrate_new_server = false;

    /*!
     * \brief have_actual_text - текст в редакторе актуальный
     */
    bool have_actual_text = false;

    /*!
     * \brief te_count - количество запущенных текстовых редакторов
     */
    std::size_t te_count = 1;

public:
    explicit DbusServer(TextEdit *te, QString session_name, QObject *parent = nullptr);
    /*!
     * \brief registrateNewServer метод для проверки факта регистрации сервиса текущим экземпляром приложения
     * \return true - зарегистрировал, false - нет
     */
    bool registrateNewServer();

    /*!
     * \brief uptedeTextInTextEdit метод запрашивающий актуальную версию текста у другого экземпляра приложения
     */
    void uptedeTextInTextEdit();

    /*!
     * \brief connectCalculateCount экземпляр зарегистрировавший сервер начинает считать подключения
     */
    void connectCalculateCount();


signals:
    /*!
     * \brief actaulText отправлка актуальной версии текста и имени фала для сохранения другим версиям приложения
     * \param pid_adresat адрес получателя
     * \param file_name имя открытого файла
     * \param html текст отображаемый в окне редактора
     */
    void actaulText(const long long pid_adresat, const QString& file_name, const QString& html);

    /*!
     * \brief getActualText сигнал для запрос актуального текста
     * \param pid номер процесса запросившего текст. В плане было сделать отправку текста только ему, но не успел
     */
    void getActualText(const long long pid);

    /*!
     * \brief textEditStart запущен ещё один экземпляр приложения в текущей сессии
     */
    void textEditStart();

    /*!
     * \brief textEditStop остановлен один экземпляр приложения в текущей сессии
     */
    void textEditStop();

public slots:
    /*!
     * \brief setDataToTextEdit установка полученных через D-Bus данных в редактор
     * \param pid_adresat если не совпадает с текущем то данные не обновляются
     * \param file_name имя редактируемого файла
     * \param html содержимое файла
     */
    void setDataToTextEdit(long long pid_adresat, const QString& file_name, const QString& html);

    /*!
     * \brief checkActualText проверка наличия актуального текста. Если он есть то отправляем запросившему
     * \param pid идентификатор процесса запросивший данные
     */
    void checkActualText(long long pid);

    /*!
     * \brief focusWindowChanged в случае если редактор получил фокус, то обновляем в нём текст
     * \param focusWindow ссылка на активное окно, пустая если приложение потеряло фокус
     */
    void focusWindowChanged(QWindow *focusWindow);

    /*!
     * \brief lastWindowClosed обработка события закрытия окна редактора
     */
    void lastWindowClosed();

    /*!
     * \brief teStart слот увеличивающий счётчик запущенных редакторов в сессии
     */
    void teStart();

    /*!
     * \brief teStop слот уменьшающий счётчик редакторов
     */
    void teStop();
};

#endif // DBUSSERVER_H
