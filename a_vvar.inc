#if defined _vvar_included
	#endinput
#endif
#define _vvar_included

#if !defined _samp_included
    #error Please include <a_samp> first.
#endif 

#if !defined _streamer_included 
    #error Please include <streamer> first.
#endif

#if !defined _INC_SSCANF
    #error Please include <sscanf2> first.
#endif

#if SSCANF_VERSION_MINOR < 9
    #error Please include <sscanf2> version 2.9.0 or high
#endif

#if defined MAX_VVAR_NAME_LEN 
    #undef MAX_VVAR_NAME_LEN
#else
    #define MAX_VVAR_NAME_LEN 41
#endif 

#if defined MAX_VVAR_STR_TYPE_LEN
    #undef MAX_VVAR_STR_TYPE_LEN
#else
    #define MAX_VVAR_STR_TYPE_LEN 41
#endif

#if !defined AREA_TYPE_VVAR
    #define AREA_TYPE_VVAR (1)
#endif

#if !defined INVALID_INT_VALUE
    #define INVALID_INT_VALUE (-1)
#endif

#if !defined IsValidVehicle
    native IsValidVehicle(vehicleid);
#endif

enum 
{
    VVAR_TYPE_NONE = -1,
    VVAR_TYPE_INT = 1,
    VVAR_TYPE_FLOAT,
    VVAR_TYPE_STRING
}

static stock IsVVarValid(vehicleid, const varname[])
{
    if(!IsValidVehicle(vehicleid))
        return 0;

    new int_data[3] = {INVALID_INT_VALUE, ...},
        var_name[MAX_VVAR_NAME_LEN],
        value_type[MAX_VVAR_STR_TYPE_LEN],
        vvar_data[95],
        bool: result = false;

    for(new idx = 1, count; idx <= Streamer_GetUpperBound(STREAMER_TYPE_AREA); idx ++)
    {
        if(!IsValidDynamicArea(idx))
            continue;

        Streamer_GetArrayData(STREAMER_TYPE_AREA, idx, E_STREAMER_EXTRA_ID, vvar_data);

        sscanf(vvar_data, "p<,>a<i>[*]s[*]s[*]", sizeof int_data, int_data, sizeof var_name, var_name, sizeof value_type, value_type);

        for(new j; j < sizeof int_data - 1; j ++)
        {
            if(int_data[j] != INVALID_INT_VALUE)
                count ++;
        }

        if(count == (sizeof int_data - 1))
        {
            if(int_data[2] != vehicleid)
                continue;

            if(strcmp(var_name, varname))
                continue;

            result = true;

            if(result)
                break;
        }
    }

    return _:result;
}

static stock IsVVarTypeValid(type)
    return (VVAR_TYPE_INT <= type <= VVAR_TYPE_STRING);

static stock SetVVarValue
(vehicleid, const varname[], type = VVAR_TYPE_INT,
int_value = INVALID_INT_VALUE, Float: float_value = 0.0, const string_value[] = EOS)
{
    if((!IsValidVehicle(vehicleid)) || (!IsVVarLenValid(varname)) || (!IsVVarTypeValid(type)))
        return 0;

    new areaid = CreateDynamicCircle
    (
        0.0, 0.0, 1.0,
        -1, -1, -1
    ),
    fmt_str[] = "%d, %d, %d, %s, %s",
    result_str[sizeof fmt_str + 
               (- 4 + 2) + 
               (- 2 + 4) + 
               (- 2 + MAX_VVAR_NAME_LEN) +
               (- 2 + MAX_VVAR_STR_TYPE_LEN)];

    if(!IsValidDynamicArea(areaid))
        return 0;
    
    switch(type)
    {
        case VVAR_TYPE_INT:
        {
            if(!IsIntValueValid(int_value))
                return 0;

            format
            (
                result_str, sizeof result_str, 
                "%d, %d, %d, %s, %d", 
                AREA_TYPE_VVAR,
                VVAR_TYPE_INT,
                vehicleid, 
                varname, 
                int_value
            );
        }

        case VVAR_TYPE_FLOAT:
        {                
            format
            (
                result_str, sizeof result_str, 
                "%d, %d, %d, %s, %f", 
                AREA_TYPE_VVAR,
                VVAR_TYPE_FLOAT,
                vehicleid, 
                varname, 
                float_value
            );
        }

        case VVAR_TYPE_STRING:
        {
            if(!IsVVarLenValid(string_value, false))
                return 0;

            format
            (
                result_str, sizeof result_str, 
                fmt_str, 
                AREA_TYPE_VVAR,
                VVAR_TYPE_STRING,
                vehicleid, 
                varname, 
                string_value
            );
        }
    }

    return Streamer_SetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, result_str);
}

static stock IsIntValueValid(value)
{
	if(-2_147_483_648 <= value <= cellmax)
		return 1;

	return 0;
}

static stock IsVVarLenValid(const string[], bool: is_name = true)
{
    new len = strlen(string),
        max_value = (is_name == true ? (MAX_VVAR_NAME_LEN - 1) : (MAX_VVAR_STR_TYPE_LEN - 1));

    if((!len) || (len > max_value))
        return 0;

    return 1;
}

static stock GetVVarValueByName
(vehicleid, const varname[], type = VVAR_TYPE_INT,
&int_value = INVALID_INT_VALUE, &Float: float_value = 0.0, const string_return[] = EOS)
{
    if((!IsValidVehicle(vehicleid)) || (!IsVVarValid(vehicleid, varname)) || (!IsVVarTypeValid(type)))
        return 0;

    new int_data[3] = {INVALID_INT_VALUE, ...},
        var_name[MAX_VVAR_NAME_LEN],
        vvar_data[95],
        bool: result = false;

    for(new idx = 1; idx <= Streamer_GetUpperBound(STREAMER_TYPE_AREA); idx ++)
    {
        if(!IsValidDynamicArea(idx))
            continue;

        Streamer_GetArrayData(STREAMER_TYPE_AREA, idx, E_STREAMER_EXTRA_ID, vvar_data);

        switch(type)
        {
            case VVAR_TYPE_INT:
                sscanf(vvar_data, "p<,>a<i>[*]s[*]d", sizeof int_data, int_data, sizeof var_name, var_name, int_value);

            case VVAR_TYPE_FLOAT:
                sscanf(vvar_data, "p<,>a<i>[*]s[*]f", sizeof int_data, int_data, sizeof var_name, var_name, float_value);

            case VVAR_TYPE_STRING:
                sscanf(vvar_data, "p<,>a<i>[*]s[*]s[*]", sizeof int_data, int_data, sizeof var_name, var_name, sizeof string_return, string_return);
        }

        if(int_data[0] != AREA_TYPE_VVAR)
            continue;

        if(int_data[1] != type)
            continue;

        if(int_data[2] != vehicleid)
            continue;

        if(strcmp(var_name, varname))
            continue;

        result = true;

        if(result)
            break;
    }

    return _:result;
}

stock SetVVarInt(vehicleid, const varname[], int_value)
    return SetVVarValue(vehicleid, varname, VVAR_TYPE_INT, int_value);

stock GetVVarInt(vehicleid, const varname[])
{
    new int_value = INVALID_INT_VALUE;

    if(!GetVVarValueByName(vehicleid, varname, VVAR_TYPE_INT, .int_value = int_value))
        return 0;

    return int_value;
}

    //1, 1, 2000, pizdapizdapizdapizdapizdapizdapizdapizda, -2147483648
    //1, 1, 2000, pizdapizdapizdapizdapizdapizdapizdapizda, pizdapizdapizdapizdapizdapizdapizdapizda

stock SetVVarFloat(vehicleid, const varname[], Float: float_value)
    return SetVVarValue(vehicleid, varname, VVAR_TYPE_FLOAT, .float_value = float_value);

stock Float: GetVVarFloat(vehicleid, const varname[])
{
    new Float: float_value = 0.0;

    if(!GetVVarValueByName(vehicleid, varname, VVAR_TYPE_FLOAT, .float_value = float_value))
        return 0;

    return Float:float_value;
}

stock SetVVarString(vehicleid, const varname[], const string_value[])
    return SetVVarValue(vehicleid, varname, VVAR_TYPE_STRING, .string_value = string_value);

stock GetVVarString(vehicleid, const varname[], const string_return[] = EOS)
    return GetVVarValueByName(vehicleid, varname, VVAR_TYPE_STRING, .string_return = string_return);

stock DeleteVVar(vehicleid, const varname[])
{
    if((!IsValidVehicle(vehicleid)) || (!IsVVarValid(vehicleid, varname)))
        return 0;

    new int_data[3] = {INVALID_INT_VALUE, ...},
        var_name[MAX_VVAR_NAME_LEN],
        vvar_data[95],
        value[MAX_VVAR_STR_TYPE_LEN],
        bool: result = false;

    for(new idx = 1; idx <= Streamer_GetUpperBound(STREAMER_TYPE_AREA); idx ++)
    {
        if(!IsValidDynamicArea(idx))
            continue;

        Streamer_GetArrayData(STREAMER_TYPE_AREA, idx, E_STREAMER_EXTRA_ID, vvar_data);

        sscanf(vvar_data, "p<,>a<i>[*]s[*]s[*]", sizeof int_data, int_data, sizeof var_name, var_name, sizeof value, value);

        if(int_data[0] != AREA_TYPE_VVAR)
            continue;

        if(int_data[2] != vehicleid)
            continue;

        if(strcmp(var_name, varname))
            continue;

        vvar_data[0] = EOS;

        result = bool:Streamer_SetArrayData(STREAMER_TYPE_AREA, idx, E_STREAMER_EXTRA_ID, vvar_data[0]);

        if(result)
        {
            DestroyDynamicArea(idx);
            break;
        }
    }

    return _:result;
}

stock GetVVarsUpperIndex(vehicleid)
{
    if(!IsValidVehicle(vehicleid))
        return 0;

    new int_data[3] = {INVALID_INT_VALUE, ...},
        var_name[MAX_VVAR_NAME_LEN],
        value[MAX_VVAR_STR_TYPE_LEN],
        vvar_data[95],
        index = INVALID_INT_VALUE;

    for(new idx = 1; idx <= Streamer_GetUpperBound(STREAMER_TYPE_AREA); idx ++)
    {
        if(!IsValidDynamicArea(idx))
            continue;

        Streamer_GetArrayData(STREAMER_TYPE_AREA, idx, E_STREAMER_EXTRA_ID, vvar_data);

        sscanf(vvar_data, "p<,>a<i>[*]s[*]s[*]", sizeof int_data, int_data, sizeof var_name, var_name, sizeof value, value);

        if(int_data[0] != AREA_TYPE_VVAR)
            continue;

        if(int_data[2] != vehicleid)
            continue;

        if(!IsVVarValid(vehicleid, var_name))
            continue;

        index = idx;
    }

    return ((index - GetVVarMinIndex(vehicleid)) + 2);
}

stock GetVVarsCount(vehicleid)
{
    if(!IsValidVehicle(vehicleid))
        return 0;

    new int_data[3] = {INVALID_INT_VALUE, ...},
        var_name[MAX_VVAR_NAME_LEN],
        vvar_data[95],
        value[MAX_VVAR_STR_TYPE_LEN],
        count;

    for(new idx = 1; idx <= Streamer_GetUpperBound(STREAMER_TYPE_AREA); idx ++)
    {
        if(!IsValidDynamicArea(idx))
            continue;

        Streamer_GetArrayData(STREAMER_TYPE_AREA, idx, E_STREAMER_EXTRA_ID, vvar_data);

        sscanf(vvar_data, "p<,>a<i>[*]s[*]s[*]", sizeof int_data, int_data, sizeof var_name, var_name, sizeof value, value);

        if(int_data[0] != AREA_TYPE_VVAR)
            continue;

        if(int_data[2] != vehicleid)
            continue;

        if(!IsVVarValid(vehicleid, var_name))
            continue;

        count ++;
    }

    return count;
}

stock GetVVarType(vehicleid, const varname[])
{
    if((!IsValidVehicle(vehicleid)) || (!IsVVarValid(vehicleid, varname)))
        return 0;

    new int_data[3] = {INVALID_INT_VALUE, ...},
        var_name[MAX_VVAR_NAME_LEN],
        vvar_data[95],
        value[MAX_VVAR_STR_TYPE_LEN],
        type = VVAR_TYPE_NONE;

    for(new idx = 1; idx <= Streamer_GetUpperBound(STREAMER_TYPE_AREA); idx ++)
    {
        if(!IsValidDynamicArea(idx))
            continue;

        Streamer_GetArrayData(STREAMER_TYPE_AREA, idx, E_STREAMER_EXTRA_ID, vvar_data);

        sscanf(vvar_data, "p<,>a<i>[*]s[*]s[*]", sizeof int_data, int_data, sizeof var_name, var_name, sizeof value, value);

        if(int_data[0] != AREA_TYPE_VVAR)
            continue;

        if(int_data[2] != vehicleid)
            continue;

        if(strcmp(var_name, varname))
            continue;

        type = int_data[1];

        if(type != VVAR_TYPE_NONE)
            break;
    }

    return type;
}

static stock GetVVarMinIndex(vehicleid)
{
    if(!IsValidVehicle(vehicleid))
        return 0;
        
    new int_data[3] = {INVALID_INT_VALUE, ...},
        var_name[MAX_VVAR_NAME_LEN],
        vvar_data[95],
        value[MAX_VVAR_STR_TYPE_LEN],
        min = INVALID_INT_VALUE;

    for(new idx = 1; idx <= Streamer_GetUpperBound(STREAMER_TYPE_AREA); idx ++)
    {
        if(!IsValidDynamicArea(idx))
            continue;

        Streamer_GetArrayData(STREAMER_TYPE_AREA, idx, E_STREAMER_EXTRA_ID, vvar_data);

        sscanf(vvar_data, "p<,>a<i>[*]s[*]s[*]", sizeof int_data, int_data, sizeof var_name, var_name, sizeof value, value);

        if(int_data[0] != AREA_TYPE_VVAR)
            continue;

        if(int_data[2] != vehicleid)
            continue;

        if(!IsVVarValid(vehicleid, var_name))
            continue;

        min = idx;

        if(min != INVALID_INT_VALUE)
            break;
    }

    return min;
}

stock GetVVarNameAtIndex(vehicleid, index, const ret_varname[])
{
    new vehicle_vvar_min_index = GetVVarMinIndex(vehicleid);

    if((!IsValidVehicle(vehicleid)) || (vehicle_vvar_min_index == INVALID_INT_VALUE))
        return 0;

    new areaid = (index + vehicle_vvar_min_index);

    Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, vvar_data);

    new int_data[3] = {INVALID_INT_VALUE, ...},
        var_name[MAX_VVAR_NAME_LEN],
        vvar_data[95],
        value[MAX_VVAR_STR_TYPE_LEN];

    sscanf(vvar_data, "p<,>a<i>[*]s[*]s[*]", sizeof int_data, int_data, sizeof var_name, var_name, sizeof value, value);

    format(ret_varname, sizeof ret_varname, "%s", var_name);

    return (strlen(ret_varname) > 0 ? strlen(ret_varname) : -1);
}

stock DeleteAllVehicleVars(vehicleid)
{
    if(!IsValidVehicle(vehicleid))
        return 0;
        
    new int_data[3] = {INVALID_INT_VALUE, ...},
        var_name[MAX_VVAR_NAME_LEN],
        vvar_data[95],
        value[MAX_VVAR_STR_TYPE_LEN],
        bool: result = false;

    for(new idx = 1; idx <= Streamer_GetUpperBound(STREAMER_TYPE_AREA); idx ++)
    {
        if(!IsValidDynamicArea(idx))
            continue;

        Streamer_GetArrayData(STREAMER_TYPE_AREA, idx, E_STREAMER_EXTRA_ID, vvar_data);

        sscanf(vvar_data, "p<,>a<i>[*]s[*]s[*]", sizeof int_data, int_data, sizeof var_name, var_name, sizeof value, value);

        if(int_data[0] != AREA_TYPE_VVAR)
            continue;

        if(int_data[2] != vehicleid)
            continue;

        if(!IsVVarValid(vehicleid, var_name))
            continue;

        vvar_data[0] = EOS;

        result = bool:Streamer_SetArrayData(STREAMER_TYPE_AREA, idx, E_STREAMER_EXTRA_ID, vvar_data[0]);

        if(result)
            DestroyDynamicArea(idx);
    }

    return _:result;
}

public OnVehicleDeath(vehicleid, killerid)
{
    DeleteAllVehicleVars(vehicleid);

    #if defined VVar__OnVehicleDeath
        return VVar__OnVehicleDeath(vehicleid, killerid);
    #else
        return 1;
    #endif
}
#if defined _ALS_OnVehicleDeath
    #undef  OnVehicleDeath
#else 
    #define _ALS_OnVehicleDeath
#endif 
#define OnVehicleDeath VVar__OnVehicleDeath
#if defined VVar__OnVehicleDeath
forward VVar__OnVehicleDeath(vehicleid, killerid);
#endif
