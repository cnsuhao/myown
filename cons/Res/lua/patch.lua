
local PATCH_MAIL_ITEM_ID = 1;

local PATCH_MIN_VERSION = 0;
local PATCH_MAX_VERSION = 1;

local function patch_mail_item_id(self)
	local mail_rec_name = "system_mail_rec";
	local item_index = 8;
	local rows = nx_get_record_rows(self, mail_rec_name);
	local item_ids = {"Item_Meridian_Lucky_2:", "Item_Meridian_Best_3_captain:", "Item_pestskill_high:", "Item_pestskill_low:"};
	
	for i=rows-1, 0, -1 do
		local items = nx_query_record_value(self, mail_rec_name, i, item_index);
		if (items ~= "") then
			for _, value in ipairs(item_ids) do
				if ( string.find( items, value) ~= nil ) then
					nx_remove_record_row(self, mail_rec_name, i);
					break;
				end
			end
		end
	end
end

function main(self, ...)
	local patch = nx_query_property(self, "PatchVersion");
	patch = nx_int(patch);

	if ( patch <  PATCH_MIN_VERSION) then
		patch = PATCH_MIN_VERSION;
	end;

	if ( patch < PATCH_MAIL_ITEM_ID ) then
		patch_mail_item_id(self);
	end

	nx_set_property(self, "PatchVersion", PATCH_MAX_VERSION);

	return "ok";
end
