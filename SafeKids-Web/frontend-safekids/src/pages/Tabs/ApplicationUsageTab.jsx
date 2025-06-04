import React, { useEffect, useState } from "react";
import axios from "axios";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Table, TableBody, TableCell, TableHead, TableHeader, TableRow } from "@/components/ui/table";
import { Tooltip, TooltipContent, TooltipProvider, TooltipTrigger } from "@/components/ui/tooltip";
import { Badge } from "@/components/ui/badge";
import { Skeleton } from "@/components/ui/skeleton";
import { Pagination, PaginationContent, PaginationItem, PaginationLink, PaginationNext, PaginationPrevious } from "@/components/ui/pagination";

export default function ApplicationUsageTab({ deviceId, selectedDate, onDateChange, isActive }) {
  const [usageData, setUsageData] = useState([]);
  const [isLoading, setIsLoading] = useState(false);
  const [filters, setFilters] = useState({
    processTitle: "",
    timeUsage: "",
    startTime: "",
  });
  const [currentPage, setCurrentPage] = useState(1);
  const itemsPerPage = 25;

  useEffect(() => {
    if (isActive && selectedDate && deviceId) {
      fetchUsageData(deviceId, selectedDate);
    }
  }, [isActive, deviceId, selectedDate]);

  useEffect(() => {
    setCurrentPage(1); // Reset to page 1 when filters or data change
  }, [filters, usageData]);

  const fetchUsageData = async (deviceId, date) => {
    setIsLoading(true);
    try {
      const token = localStorage.getItem("token");
      const response = await axios.post(
        `/api/devices/${deviceId}/process-usage`,
        { dateRecorded: date },
        {
          headers: {
            Authorization: `Bearer ${token}`,
          },
        }
      );
      setUsageData(response.data.data || []);
    } catch (error) {
      console.error("Error fetching process usage data:", error);
      setUsageData([]);
    } finally {
      setIsLoading(false);
    }
  };

  const handleFilterChange = (field, value) => {
    setFilters((prev) => ({ ...prev, [field]: value }));
  };

  const filteredData = usageData.filter((entry) => {
    const matchesTitle = entry.process_title
      .toLowerCase()
      .includes(filters.processTitle.toLowerCase());
    const matchesTimeUsage =
      filters.timeUsage === "" || entry.time_usage >= parseFloat(filters.timeUsage);
    const matchesStartTime =
      filters.startTime === "" ||
      entry.start_time.toLowerCase().startsWith(filters.startTime.toLowerCase());
    return matchesTitle && matchesTimeUsage && matchesStartTime;
  });

  // Calculate pagination
  const totalPages = Math.ceil(filteredData.length / itemsPerPage);
  const startIndex = (currentPage - 1) * itemsPerPage;
  const endIndex = startIndex + itemsPerPage;
  const paginatedData = filteredData.slice(startIndex, endIndex);

  const handlePageChange = (page) => {
    setCurrentPage(page);
  };

  return (
    <div className="space-y-6">
      {/* Filters and Date Selector */}
      <Card>
        <CardContent className="grid gap-4 mt-4">
          <div className="grid grid-cols-1 md:grid-cols-4 gap-4">
            <div>
              <Label htmlFor="date">Chọn ngày</Label>
              <Input
                id="date"
                type="date"
                value={selectedDate}
                onChange={(e) => onDateChange(e.target.value)}
              />
            </div>
            <div>
              <Label htmlFor="filter-title">Tên ứng dụng</Label>
              <Input
                id="filter-title"
                placeholder="Nhập tên ứng dụng"
                value={filters.processTitle}
                onChange={(e) => handleFilterChange("processTitle", e.target.value)}
              />
            </div>
            <div>
              <Label htmlFor="filter-time">Thời gian sử dụng (phút)</Label>
              <Input
                id="filter-time"
                type="number"
                placeholder="Nhập số phút"
                value={filters.timeUsage}
                onChange={(e) => handleFilterChange("timeUsage", e.target.value)}
                min="0"
              />
            </div>
            <div>
              <Label htmlFor="filter-start">Giờ bắt đầu (HH:MM)</Label>
              <Input
                id="filter-start"
                placeholder="Nhập giờ HH:MM"
                value={filters.startTime}
                onChange={(e) => handleFilterChange("startTime", e.target.value)}
              />
            </div>
          </div>
        </CardContent>
      </Card>

      {/* Usage Data */}
      <Card>
        <CardContent>
          {isLoading ? (
            <div className="space-y-2">
              <Skeleton className="h-8 w-full" />
              <Skeleton className="h-8 w-full" />
              <Skeleton className="h-8 w-full" />
            </div>
          ) : filteredData.length > 0 ? (
            <>
              <Table>
                <TableHeader>
                  <TableRow>
                    <TableHead>Tên ứng dụng</TableHead>
                    <TableHead>Thời gian sử dụng</TableHead>
                    <TableHead>Giờ bắt đầu</TableHead>
                  </TableRow>
                </TableHeader>
                <TableBody>
                  {paginatedData.map((entry, index) => (
                    <TableRow key={index}>
                      <TableCell>
                        <TooltipProvider>
                          <Tooltip>
                            <TooltipTrigger asChild>
                              <span className="cursor-pointer">{entry.process_title}</span>
                            </TooltipTrigger>
                            <TooltipContent>
                              <p className="font-medium">{entry.process_title}</p>
                              <p>Giờ bắt đầu: {entry.start_time}</p>
                              <p>Thời gian sử dụng: {entry.time_usage} phút</p>
                            </TooltipContent>
                          </Tooltip>
                        </TooltipProvider>
                      </TableCell>
                      <TableCell>
                        <Badge
                          variant={entry.time_usage > 120 ? "destructive" : "default"}
                          className={entry.time_usage > 120 ? "bg-red-500" : "bg-blue-500"}
                        >
                          {entry.time_usage} phút
                        </Badge>
                      </TableCell>
                      <TableCell>{entry.start_time}</TableCell>
                    </TableRow>
                  ))}
                </TableBody>
              </Table>
              {/* Pagination */}
              {totalPages > 1 && (
                <Pagination className="mt-4">
                  <PaginationContent>
                    <PaginationItem>
                      <PaginationPrevious
                        onClick={() => handlePageChange(currentPage - 1)}
                        disabled={currentPage === 1}
                      />
                    </PaginationItem>
                    {[...Array(totalPages)].map((_, index) => (
                      <PaginationItem key={index}>
                        <PaginationLink
                          onClick={() => handlePageChange(index + 1)}
                          isActive={currentPage === index + 1}
                        >
                          {index + 1}
                        </PaginationLink>
                      </PaginationItem>
                    ))}
                    <PaginationItem>
                      <PaginationNext
                        onClick={() => handlePageChange(currentPage + 1)}
                        disabled={currentPage === totalPages}
                      />
                    </PaginationItem>
                  </PaginationContent>
                </Pagination>
              )}
            </>
          ) : (
            <div className="text-center text-muted-foreground py-4">
              Không có dữ liệu sử dụng cho ngày đã chọn hoặc bộ lọc.
            </div>
          )}
        </CardContent>
      </Card>
    </div>
  );
}