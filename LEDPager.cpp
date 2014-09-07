#include <iostream>
#include <unordered_map>
#include <string>
#include <gloox/vcardmanager.h>
#include <gloox/vcardhandler.h>
#include <gloox/rostermanager.h>
#include <gloox/rosteritem.h>
#include <gloox/client.h>
#include <gloox/message.h>
#include <gloox/messagehandler.h>
#include <gloox/connectionlistener.h>

using namespace std;
using namespace gloox;

class LEDPager : public MessageHandler, VCardHandler, ConnectionListener {

  public: 
  LEDPager (string newjid, string password, void (*onRecvFunc)(void), int debug)
  {
    DEBUG = debug;
    JID jid(newjid);
    onRecv = onRecvFunc;
    client = new Client( jid, password );
    vCardManager = new VCardManager(client);
    client->registerMessageHandler( this );
    client->registerConnectionListener(this);
    this->connect(false);
    barejid = jid.bare();
  }

  void handleVCard  (const JID& jid, const VCard* vcard )   
  {
    cout << barejid << " - got vcard for jid: " << jid.bare() << endl;
    if (DEBUG) {
      cout << "-" << endl;
      cout << "nickname: " << vcard->formattedname() << endl;
      cout << "-" << endl;
    }
    vCardIndex.emplace(jid.username(), vcard->formattedname());
  }

  void handleVCardResult ( VCardContext context, const JID& jid, StanzaError se = StanzaErrorUndefined )
  {
    cout << barejid << " - got error processing vcard for " << jid.bare() << endl;
  }
  void handleMessage( const Message& stanza, MessageSession* session = 0 ) 
  {
    auto vCardName = vCardIndex.find(stanza.from().username());
    RosterItem* myItem = client->rosterManager()->getRosterItem(stanza.from());
    if (vCardName == vCardIndex.end()) {
      vCardManager->fetchVCard(stanza.from(), this);
      /* todo: we may want to block until the vcard is resolved... or we may not... remains to be seen */
      cout << barejid << " - Received message: " << (myItem ? myItem->jidJID().bare() : stanza.from().bare()) << " - " << stanza.body() << endl;
    } 
    else {
      cout << barejid << " - Received message: "<< vCardName->second << " - " << stanza.body() << endl;
    }
    if (stanza.body().size() != 0) {
      /* ffs I don't care about typing notifications */
      onRecv();
    }
    if (DEBUG) {
      cout << "Raw debug output: " << stanza.tag()->xml() << endl;
    }
  }

  void onConnect() {
    std::cout << barejid << " - ConnListener::onConnect()" << endl;
  }

  void onDisconnect(ConnectionError e) {
    std::cout << barejid << " - ConnListener::onDisconnect() " << e << endl;
    client->connect(false);
  }

  bool onTLSConnect(const CertInfo& info) {
    std::cout << barejid << " - ConnListener::onTLSConnect()" << endl;
    return true;
  }

  /* wrappers around the client connection functions */
  void recv (int timeout)
  {
    client->recv(timeout);
  }

  void connect (bool blocking)
  {
    client->connect(blocking);
  }

  void disconnect ()
  {
    client->disconnect();
  }

  private:
  string barejid;
  Client* client;
  int DEBUG;
  unordered_map<string, string> vCardIndex;
  VCardManager* vCardManager;
  void (*onRecv)(void);
};

/* vim: et ts=2 sw=2
*/

