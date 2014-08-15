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
    client->connect(true);
  }

  void handleVCard  (const JID& jid, const VCard* vcard )   
  {
    cout << "got vcard for jid: " << jid.bare() << endl;
    if (DEBUG) {
      cout << "-" << endl;
      cout << "nickname: " << vcard->formattedname() << endl;
      cout << "-" << endl;
    }
    vCardIndex.emplace(jid.username(), vcard->formattedname());
  }

  void handleVCardResult ( VCardContext context, const JID& jid, StanzaError se = StanzaErrorUndefined )
  {
    cout << "got error processing vcard for " << jid.bare() << endl;
  }
  void handleMessage( const Message& stanza, MessageSession* session = 0 ) 
  {
    auto vCardName = vCardIndex.find(stanza.from().username());
    RosterItem* myItem = client->rosterManager()->getRosterItem(stanza.from());
    if (vCardName == vCardIndex.end()) {
      vCardManager->fetchVCard(stanza.from(), this);
      /* todo: we may want to block until the vcard is resolved... or we may not... remains to be seen */
      cout << "Received message: "<< myItem->jidJID().bare() << " - " << stanza.body() << endl;
    } 
    else {
      cout << "Received message: "<< vCardName->second << " - " << stanza.body() << endl;
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
    std::cout << "ConnListener::onConnect()" << endl;
  }

  void onDisconnect(ConnectionError e) {
    std::cout << "ConnListener::onDisconnect() " << e << endl;
    client->connect(true);
  }

  bool onTLSConnect(const CertInfo& info) {
    std::cout << "ConnListener::onTLSConnect()" << endl;
    return true;
  }
  private:
  Client* client;
  int DEBUG;
  unordered_map<string, string> vCardIndex;
  VCardManager* vCardManager;
  void (*onRecv)(void);
};

