#include <dpp/dpp.h>
#include <sstream>
#include <unordered_set>

const std::string BOT_TOKEN = "Input your token here";
std::unordered_set<int> online_users; // id of online users
bool enable_trace = false;
 
void log_on_cout(std::string s){
    std::cout << "[" << dpp::utility::current_date_time() << "] " << "DEBUG: " << s << "\n";
}

void log_on_cout_trace(std::string s){
    if(enable_trace) std::cout << "[" << dpp::utility::current_date_time() << "] " << "Trace: " << s << "\n";
}

// Modifies the online_users hashset, which contains the id of all online users
// todo: the presence_status supports whether the person is using a mobile device.
void change_user_status(dpp::presence_status& ps, int user_id){
    log_on_cout_trace("Triggered change_user_status");
    switch(ps){
        case dpp::presence_status::ps_offline: case dpp::presence_status::ps_idle:
            online_users.erase(user_id);
            break;
        case dpp::presence_status::ps_online: case dpp::presence_status::ps_dnd:
            online_users.insert(user_id);
            break;
    }
}

int main() {
    uint64_t intents = dpp::i_all_intents;
    dpp::cluster bot(BOT_TOKEN, intents);
    
    bot.on_log(dpp::utility::cout_logger());

    // events
    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "greeting") {
            log_on_cout("Triggered Slash Command greeting");
            event.reply("Hello");
        }
        else if (event.command.get_command_name() == "pingactive") {
            // ping all the online members, including people with do not disturb status
            const dpp::interaction &interaction = event.command;
            log_on_cout("Triggered Slash Command pingactive");

            dpp::channel cur_channel = interaction.get_channel();
            std::__1::map<dpp::snowflake, dpp::guild_member *> snowflake_to_guild = cur_channel.get_members();

            log_on_cout_trace("pingactive: There are this many users: "  + std::to_string(snowflake_to_guild.size()));
            log_on_cout_trace("pingactive: There are this many online users" + std::to_string(online_users.size()));

            std::stringstream ss;

            for(auto[snowflake, guild] : snowflake_to_guild){
                dpp::user cur_user = *guild->get_user();
                if(online_users.find(cur_user.id) != online_users.end() && !cur_user.is_bot()) {
                    ss << cur_user.get_mention();
                }
            }
            event.reply(ss.str());
        }
    });

    bot.on_presence_update([](const dpp:: presence_update_t& event){
        log_on_cout("Triggered on_presence_update");
        dpp::presence user_presence = event.rich_presence;
        dpp::presence_status user_status = user_presence.status();

        change_user_status(user_status, user_presence.user_id);
    });

    bot.on_guild_create([](const dpp::guild_create_t& event) {
        dpp::presence_map pm = event.presences;
        log_on_cout("Triggered on_guild_create");
        log_on_cout_trace("on_guild_create: there are this many users: " + std::to_string(pm.size()));

        for(auto [snowflake, presence] : pm){
            dpp::presence_status user_status = presence.status();
            change_user_status(user_status, presence.user_id);
        }
    });

    // on_ready is run once the bot is connected
    // The square brackets specify which variables are "captured" by the lambda, and how (by value or reference).
    bot.on_ready([&bot](const dpp::ready_t& event) {
        // Registration in template called dpp::run_once, prevents re-run every time your bot does a full reconnection
        // Register all the slash commands
        bot.global_command_create(dpp::slashcommand("greeting", "Says hello back to you", bot.me.id));
        bot.global_command_create(dpp::slashcommand("pingactive", "Ping all active members", bot.me.id));
    });


    bot.start(dpp::st_wait);
}
